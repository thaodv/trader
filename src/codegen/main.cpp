#include "stdafx.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StringTokenizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>


using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::DirectoryIterator;
using Poco::File;
using Poco::Path;


class CodeGenApp : public Application
{
public:
    CodeGenApp()
    {
    }

protected:
    void defineOptions(OptionSet& options)
    {
        Application::defineOptions(options);

        options.addOption(
            Option("help", "h", "Display help information on command line arguments.")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<CodeGenApp>(this, &CodeGenApp::handleHelp)));

        options.addOption(
            Option("outputdir", "o", "Location to write cpp and header files")
            .required(false)
            .repeatable(false)
            .argument("output directory")
            .callback(OptionCallback<CodeGenApp>(this, &CodeGenApp::handleOutputDir)));

        options.addOption(
            Option("inputdir", "i", "Location of the API JSON files")
            .required(false)
            .repeatable(false)
            .argument("input directory")
            .callback(OptionCallback<CodeGenApp>(this, &CodeGenApp::handleInputDir)));

        options.addOption(
            Option("namespace", "n", "Namespace")
            .required(false)
            .repeatable(false)
            .argument("input directory")
            .callback(OptionCallback<CodeGenApp>(this, &CodeGenApp::handleNamespace)));
    }

    void handleHelp(const std::string& name, const std::string& value)
    {
        displayHelp();
        stopOptionsProcessing();
    }

    void handleInputDir(const std::string& name, const std::string& value)
    {
        _inputDir = value;
    }

    void handleOutputDir(const std::string& name, const std::string& value)
    {
        _outputDir = value;
    }

    void handleNamespace(const std::string& name, const std::string& value)
    {
        _namespace = value;
    }

    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A simple command line client for posting status updates.");
        helpFormatter.format(std::cout);
    }

	class FileOutputStream;

	class tabs 
	{
		std::size_t _n;
	public:
		explicit tabs(std::size_t n) : _n(n) {}
		std::size_t getn() const { return _n; }
		friend FileOutputStream& operator<<(FileOutputStream& os, const tabs& obj)
		{
			std::size_t n = obj.getn();
			for (std::size_t i = 0; i < n; ++i)
				os.tempStream << "    ";
			return os;
		}
	};

    class FileOutputStream
    {
    public:

        FileOutputStream& operator++()
        {
            // actual increment takes place here
            indentation++;
            return *this;
        }

        FileOutputStream& operator--()
        {
            // actual increment takes place here
			poco_assert(indentation > 0);
            indentation--;
            return *this;
        }

        FileOutputStream(const std::string& path, std::ios::openmode mode = std::ios::out | std::ios::trunc)
            : stream(path, mode)
            ,indentation(0)
        {}

        class tabs 
        {
            std::size_t _n;
        public:
            explicit tabs(std::size_t n) : _n(n) {}
            std::size_t getn() const { return _n; }
            friend std::ostream& operator<<(std::ostream& os, const tabs& obj)
            {
                std::size_t n = obj.getn();
                for (std::size_t i = 0; i < n; ++i)
                    os << "    ";
                return os;
            }
        };

        friend FileOutputStream& operator<<(FileOutputStream& os, const char* text)
        {
            os.tempStream << text;
            return os;
        }

        friend FileOutputStream& operator<<(FileOutputStream& os, std::string& str)
        {
            os.tempStream << str;
            return os;
        }

		friend FileOutputStream& operator<<(FileOutputStream& os, const std::string& str)
		{
			os.tempStream << str;
			return os;
		}

        friend FileOutputStream& operator<<(FileOutputStream& os, FileOutputStream& (*_Pfn)(FileOutputStream&))
        {// call basic_ostream manipulator
            return ((*_Pfn)(os));
        }

        friend FileOutputStream & endl(FileOutputStream & os)
        {
            os.stream << std::endl;
            os.stream << tabs(os.indentation);
            os.stream << os.tempStream.str();
            os.tempStream.str("");
            os.tempStream.clear();
            return os;
        }

		friend FileOutputStream & cendl(FileOutputStream & os)
		{
			os.stream << std::endl;
			os.stream << tabs(os.indentation);
			os.stream << os.tempStream.str();
			os.stream << ";";
			os.tempStream.str("");
			os.tempStream.clear();
			return os;
		}

        Poco::Int32 indentation;
        Poco::FileOutputStream stream;
        std::ostringstream tempStream;
    };

	struct ObjectSchemaDefinition
	{
		void read(Poco::SharedPtr<Poco::JSON::Object> obj)
		{
			rootObj = obj;
		}

		void writeCpp(FileOutputStream& cpp)
		{

		}

		void writeHeader(FileOutputStream& header)
		{
			std::vector<std::string> propertyNames;
			rootObj->getNames(propertyNames);
			for (auto& it : propertyNames)
			{
				Poco::JSON::Object::Ptr propertyVal = rootObj->getObject(it);
				std::string type = propertyVal->get("type");
				if (type.compare("double")==0)
				{
					header << "double" << tabs(1) << it << cendl;
					newLine(header);
				}
			}
		}

		std::string name;
		Poco::SharedPtr<Poco::JSON::Object> rootObj;
	};

	struct EndPoint
	{
		enum Method {
			POST,
			GET
		};
		std::string url;
		Method method;
		std::string description;
		std::string name;
		std::string responseSchemaName;

		void read(Poco::SharedPtr<Poco::JSON::Object> obj)
		{
			name = obj->getValue<std::string>("title");
			description = obj->getValue<std::string>("description");
			Poco::JSON::Object::Ptr targetSchema = obj->getObject("targetSchema");
			std::string schemaRef = targetSchema->getValue<std::string>("$ref");
			url = obj->getValue<std::string>("href");
			std::string method = obj->get("method");
			if (method.compare("get") != 0)
			{
				method = GET;
			}
			else
			{
				method = POST;
			}
			Poco::StringTokenizer str(schemaRef, "/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
			responseSchemaName = str[str.count()-1];
		}

		void writeCpp(FileOutputStream& cpp, const std::string& apiName)
		{
			cpp << "Poco::AutoPtr<" << responseSchemaName << "> " << apiName << "::" << name;
			if (method == GET)
			{
				cpp << "()";
			}
			{
				scopedStream stream(cpp);
				cpp << "Poco::AutoPtr<" << responseSchemaName << "> retVal = new " << responseSchemaName << cendl;
				cpp << "Poco::AutoPtr<Poco::Util::AbstractConfiguration> pResult = invoke(Poco::Net::HTTPRequest::";
				if (method == GET) cpp << "HTTP_GET"; else cpp << "HTTP_POST";
				cpp << "\"" << name << "\"";
				cpp << cendl;
				cpp << "retVal->read(pResult)" << cendl;
				cpp << "return retVal" << cendl;
			}
		}

		void writeHeader(FileOutputStream& header)
		{
			header << endl;
			header << "// " << description << endl;
			header << "Poco::AutoPtr<" << responseSchemaName << "\> " << name;
			if (method = GET)
			{
				header << "()";
			}
			header << cendl;
		}
	};

	void getAPIName(const std::string& apiFile, std::string& apiName)
	{
		Path p(apiFile);
		apiName = p.getBaseName();
	}

	void pushNameSpace(FileOutputStream& stream, std::string& namespacename)
	{
		if (!namespacename.empty())
		{
			stream << "namespace " << namespacename << " {" << endl;
			stream << endl;
			++stream;
		}
	}

	void popNameSpace(FileOutputStream& stream, std::string& namespacename)
	{
		if (!_namespace.empty())
		{
			--stream;
			stream << "} //" << namespacename << endl;
		 }
	}

	void newLine(FileOutputStream& stream)
	{
		stream << endl;
	}

	class scopedStream
	{
	public:
		enum IndentType
		{
			INC = 0,
			DEC
		};
		
		scopedStream(FileOutputStream& stream, bool brackets = true, IndentType indent = INC)
			: _stream(stream)
			, _indent(indent)
			, _brackets(brackets)
		{
			if (brackets) stream << "{" << endl;
			if (indent == INC) ++stream; else --stream;
		}

		~scopedStream()
		{
			if (_indent == INC) --_stream; else ++_stream;
			if (_brackets) _stream << "}" << endl;
		}

		IndentType _indent;
		FileOutputStream& _stream;
		bool _brackets;
	};

	void pushClass(FileOutputStream& stream, std::string& className, const char** parentClass , Poco::UInt32 length)
	{
		stream << "class " << className;
		Poco::UInt32 count = 0;
		for(Poco::UInt32 count = 0; count < length; count++)
		{
			if (count) stream << ", ";
			else stream << " : ";
			stream << " public" << parentClass[count];
			count++;
		}
		stream << " {";
		newLine(stream);
		++stream;

		//public section 
		{
			scopedStream scoped(stream, false, scopedStream::DEC);
			stream << "public:" << endl;
		}
		//declare constructor
		newLine(stream);
		stream << "// Constructor" << endl;
		stream << className << "()" << cendl;

		//declare destructor
		newLine(stream);
		stream << "// Destructor" << endl;
		stream << "~" << className << "()" << cendl;
	}

	void popClass(FileOutputStream& stream, std::string& className)
	{
		if (!className.empty())
		{
			--stream;
			stream << "}; //" << className << endl;
			newLine(stream);
		}
	}

	void startHeader(FileOutputStream& stream, const char** headerFile, Poco::UInt32 length)
	{
		stream << "#pragma once " << endl;
		Poco::UInt32 count = 0;
		for (Poco::UInt32 count = 0; count < length; count++)
		{
			stream << "#include \"" << headerFile[count] << "\"";
		}
		newLine(stream);
	}

	void startCpp(FileOutputStream& stream, const char** headerFile, Poco::UInt32 length)
	{
		stream << "#include \"stdafx.h\" " << endl;
		Poco::UInt32 count = 0;
		for (Poco::UInt32 count = 0; count < length; count++)
		{
			stream << "#include \"" << headerFile[count] << "\"" << endl;
		}
		newLine(stream);
	}

    void processTemplate(const std::string& apiFile)
    {
        std::ostringstream ostr;
        Poco::FileInputStream fis(apiFile);
        Poco::StreamCopier::copyStream(fis, ostr);

        Poco::UInt32 headerIdent = 0;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(ostr.str());
        Poco::JSON::Object::Ptr api = result.extract<Poco::JSON::Object::Ptr>();

		std::string apiName;
		getAPIName(apiFile, apiName);

		Poco::JSON::Object::Ptr definitions = api->getObject("definitions");
		std::map<std::string, ObjectSchemaDefinition> schemaDefinitions;
		std::vector<std::string> definitionNames;
		definitions->getNames(definitionNames);
		for (auto& it : definitionNames)
		{
			Poco::JSON::Object::Ptr definition = definitions->getObject(it);
			Poco::JSON::Object::Ptr properties = definition->getObject("properties");
			ObjectSchemaDefinition def;
			def.read(properties);
			def.name = it;
			std::string key = "#/definitions/" + it;
			schemaDefinitions.insert(std::pair<std::string,ObjectSchemaDefinition>(key, def));
		}

		Poco::JSON::Array::Ptr methods = api->getArray("links");
		std::vector<EndPoint> endPoints;
		for (auto& it : *methods)
		{
			endPoints.emplace_back();
			EndPoint& endPoint = endPoints.back();
			endPoint.read(it.extract<Poco::JSON::Object::Ptr>());
		}

		//Write
        std::string headerFileName = _outputDir + Path::separator() + apiName + ".h";
		File headerFile(headerFileName);
		FileOutputStream header(headerFileName, std::ios::out);

        std::string cppFileName = _outputDir + Path::separator() + apiName + ".cpp";
        FileOutputStream cpp(cppFileName, std::ios::out);

		startHeader(header, {}, 0);
		newLine(header);
		pushNameSpace(header, _namespace);
			for (auto& schemaDefinition : schemaDefinitions)
			{
				ObjectSchemaDefinition& def = schemaDefinition.second;
				pushClass(header, def.name, {}, 0);
				def.writeHeader(header);
				popClass(header, def.name);
			}
			pushClass(header, apiName, {}, 0);
			for (auto& endPoint : endPoints)
			{
				endPoint.writeHeader(header);
			}
			popClass(header, apiName);
		popNameSpace(header, _namespace);

		const char* cppHeaders[1];
		cppHeaders[0] = headerFileName.c_str();
		startCpp(cpp, cppHeaders, 1);
		pushNameSpace(cpp, _namespace);
		for (auto& endPoint : endPoints)
		{
			endPoint.writeCpp(cpp, apiName);
		}
		popNameSpace(cpp, _namespace);
    

    }

    void processTemplates(const std::string& root)
    {
        DirectoryIterator it(root);
        DirectoryIterator end;
        while (it != end)
        {
            if (it->isDirectory())
            {
                processTemplates(it->path());
            }
            else
            {
                Path p(it->path());
                if (p.getExtension().compare("json") == 0)
                {
                    processTemplate(it->path());
                }
            }
            ++it;
        }
    }

    int main(const std::vector<std::string>& args)
    {
        try
        {
            if (_inputDir.empty())
            {
                _inputDir = Path::current() + "apis" + Path::separator();
            }

            if (_outputDir.empty())
            {
                _outputDir = Path::current() + "codegen" + Path::separator();
            }

			File outputDir(_outputDir);
			outputDir.createDirectories();

            processTemplates(_inputDir);
        }
        catch (Poco::Exception& exc)
        {
            std::cerr << exc.displayText() << std::endl;
            return Application::EXIT_SOFTWARE;
        }
        return Application::EXIT_OK;
    }

private:
    std::string _inputDir;
    std::string _outputDir;
    std::string _namespace;
};

POCO_APP_MAIN(CodeGenApp)

