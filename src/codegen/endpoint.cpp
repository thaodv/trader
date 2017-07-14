#include "stdafx.h"
#include "fileoutputstream.h"
#include "endpoint.h"
#include "helpers.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::DirectoryIterator;
using Poco::File;
using Poco::Path;
using namespace Poco;
using namespace std;

namespace trader {

	void EndPoint::read(JSON::Object::Ptr obj)
	{
		name = obj->getValue<string>("title");
		description = obj->getValue<string>("description");
		url = obj->getValue<string>("href");
		string methodString = obj->get("method");
		if (methodString.compare("GET") == 0)
		{
			method = GET;
		}
		else
		{
			method = POST;
		}
		JSON::Array::Ptr targetSchemas = obj->getArray("targetSchemas");
		for (auto targetSchemaVar : *targetSchemas)
		{
			JSON::Object::Ptr targetSchema = targetSchemaVar.extract<JSON::Object::Ptr>();
			string targetSchemaRef = targetSchema->getValue<string>("$ref");
			StringTokenizer str(targetSchemaRef, "/", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
			responseSchemaNames.push_back(str[str.count() - 1]);
		}
		JSON::Object::Ptr schema = obj->getObject("schema");
		if (schema)
		{
			string schemaRef = schema->getValue<string>("$ref");
			StringTokenizer str2(schemaRef, "/", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
			inputSchemaName = str2[str2.count() - 1];
		}
	}

	void EndPoint::writeResponseSchema(ApiFileOutputStream& cpp, UInt32 idx)
	{
		if (idx < responseSchemaNames.size())
		{
			cpp << "try " ;
			{
				ScopedStream<ApiFileOutputStream> stream(cpp);
				cpp << "Poco::AutoPtr<" << responseSchemaNames[idx] << "> retVal = new " << responseSchemaNames[idx] << "()" << cendl;
				cpp << "retVal->read(pResult)" << cendl;
				if (idx == 0)
				{
					cpp << "return retVal" << cendl;
				}
			}
			cpp << "catch (Poco::Exception& exc) ";
			{
				ScopedStream<ApiFileOutputStream> stream(cpp);
				cpp << "(void)exc" << cendl;
				writeResponseSchema(cpp, idx + 1);
			}
		}
		else
		{
			cpp << "Poco::Logger::get(\"Logs\").information(\"Response Parsing Error: %s\", exc.displayText())" << cendl;
		}
	}

	void EndPoint::writeCpp(ApiFileOutputStream& cpp)
	{
		cpp << "Poco::AutoPtr<" << responseSchemaNames[0] << "> " << _config.apiName << "::" << name;
		cpp << "(";
		if (!inputSchemaName.empty())
		{
			cpp << "Poco::AutoPtr<" << inputSchemaName << "> " << var_name(inputSchemaName);
		}
		cpp << ") ";
		{
			ScopedStream<ApiFileOutputStream> stream(cpp);
			cpp << "Poco::URI uri(_uri + \"" << url << "\" )" << cendl;
			if (!inputSchemaName.empty())
			{
				string inputSchemaRefStr = "#/definitions/" + inputSchemaName;
				auto found = _config.schemaDefinitions.find(inputSchemaRefStr);
				if (found != _config.schemaDefinitions.end())
				{
					ObjectSchemaDefinition& obj = found->second;
					obj.writeRestEncodedParams(cpp);
				}
			}
			cpp << "Poco::Dynamic::Var pResult = invoke(Poco::Net::HTTPRequest::";
			if (method == GET) cpp << "HTTP_GET"; else cpp << "HTTP_POST";
			cpp << ", uri)" << cendl;
			writeResponseSchema(cpp, 0);
			cpp << "Poco::AutoPtr<" << responseSchemaNames[0] << "> retVal = new " << responseSchemaNames[0] << "()" << cendl;
			cpp << "return retVal" << cendl;
		}
		cpp << endl;
	}

	void EndPoint::writeHeader(ApiFileOutputStream& header)
	{
		header << endl;
		header << "// " << description << endl;
		header << "Poco::AutoPtr<" << responseSchemaNames[0] << "> " << name;
		header << "(";
		if (!inputSchemaName.empty())
		{
			header << "Poco::AutoPtr<" << inputSchemaName << "> " << var_name(inputSchemaName);
		}
		header << ")";
		header << cendl;
	}
}


