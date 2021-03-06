#pragma once

namespace std
{
    inline ostringstream &cendl(ostringstream &os)
    {
        os << std::endl;
        os << ";";
        return os;
    }
    inline std::ostringstream &dot(std::ostringstream &os)
    {
        if (os.str().length())
        {
            os << ".";
        }
        return os;
    }

    inline std::ostringstream &operator<<(std::ostringstream &os, std::ostringstream &(*_Pfn)(std::ostringstream &))
    {
        return ((*_Pfn)(os));
    }
} // namespace std

namespace trader
{

    using Poco::DirectoryIterator;
    using Poco::File;
    using Poco::Path;
    using Poco::Util::AbstractConfiguration;
    using Poco::Util::Application;
    using Poco::Util::HelpFormatter;
    using Poco::Util::Option;
    using Poco::Util::OptionCallback;
    using Poco::Util::OptionSet;
    using namespace Poco;
    using namespace std;

    class ApiFileOutputStream;
    class ApiStreamBuffer;

    class tabs
    {
        size_t _n;

      public:
        explicit tabs(size_t n)
            : _n(n)
        {
        }
        size_t getn() const { return _n; }

        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const tabs &obj);
        friend ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const tabs &obj);
        friend std::ostringstream &operator<<(std::ostringstream &os, const tabs &obj);
        friend std::ostream &operator<<(ostream &os, const tabs &obj);
    };

    class ApiFileOutputStream
    {
      public:
        ApiFileOutputStream &operator++()
        {
            indentation++;
            return *this;
        }

        ApiFileOutputStream &operator--()
        {

            poco_assert(indentation > 0);
            indentation--;
            return *this;
        }

        ApiFileOutputStream(const string &path, ios::openmode mode = ios::out | ios::trunc)
            : indentation(0)
            , stream(path, mode)
        {
        }

        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const char *text);
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, string &str);
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const string &str);
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const Int32 &num);
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os,
                                               ApiFileOutputStream &(*_Pfn)(ApiFileOutputStream &));
        friend ApiFileOutputStream &endl(ApiFileOutputStream &os);
        friend ApiFileOutputStream &cendl(ApiFileOutputStream &os);

        Int32 indentation;
        Poco::FileOutputStream stream;
        ostringstream tempStream;
    };

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const char *text)
    {
        os.tempStream << text;
        return os;
    }

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, string &str)
    {
        os.tempStream << str;
        return os;
    }

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const string &str)
    {
        os.tempStream << str;
        return os;
    }

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const Int32 &num)
    {
        os.tempStream << num;
        return os;
    }

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, ApiFileOutputStream &(*_Pfn)(ApiFileOutputStream &))
    {
        return ((*_Pfn)(os));
    }

    inline ApiFileOutputStream &endl(ApiFileOutputStream &os)
    {
        os.stream << std::endl;
        os.stream << tabs(os.indentation);
        os.stream << os.tempStream.str();
#ifdef DEBUG
        cout << os.tempStream.str() << std::endl;
#endif
        os.tempStream.str("");
        os.tempStream.clear();
        return os;
    }

    inline ApiFileOutputStream &cendl(ApiFileOutputStream &os)
    {
        os.stream << std::endl;
        os.stream << tabs(os.indentation);
        os.stream << os.tempStream.str();
        os.stream << ";";
#ifdef DEBUG
        cout << os.tempStream.str() << std::endl;
#endif
        os.tempStream.str("");
        os.tempStream.clear();
        return os;
    }

    class ApiStreamBuffer
    {
      public:
        ApiStreamBuffer &operator++()
        {
            indentation++;
            return *this;
        }

        ApiStreamBuffer &operator--()
        {

            poco_assert(indentation > 0);
            indentation--;
            return *this;
        }

        ApiStreamBuffer(ApiFileOutputStream &fileStream) { indentation = fileStream.indentation; }

        friend ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const char *text);
        friend ApiStreamBuffer &operator<<(ApiStreamBuffer &os, string &str);
        friend ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const string &str);
        friend ApiStreamBuffer &operator<<(ApiStreamBuffer &os, ApiStreamBuffer &(*_Pfn)(ApiStreamBuffer &));
        friend ApiStreamBuffer &endl(ApiStreamBuffer &os);
        friend ApiStreamBuffer &cendl(ApiStreamBuffer &os);

        inline ApiStreamBuffer &dot(ApiStreamBuffer &os)
        {
            os.tempStream << ".";
            return os;
        }

        std::string str() { return tempStream.str(); }

        Int32 indentation;
        ostringstream tempStream;
    };

    inline ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const char *text)
    {
        os.tempStream << text;
        return os;
    }

    inline ApiStreamBuffer &operator<<(ApiStreamBuffer &os, string &str)
    {
        os.tempStream << str;
        return os;
    }

    inline ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const string &str)
    {
        os.tempStream << str;
        return os;
    }

    inline ApiStreamBuffer &operator<<(ApiStreamBuffer &os, ApiStreamBuffer &(*_Pfn)(ApiStreamBuffer &))
    {
        return ((*_Pfn)(os));
    }

    inline ApiStreamBuffer &endl(ApiStreamBuffer &os)
    {
        os.tempStream << std::endl;
        os.tempStream << tabs(os.indentation);
        return os;
    }

    inline ApiStreamBuffer &cendl(ApiStreamBuffer &os)
    {
        os.tempStream << ";";
        os.tempStream << std::endl;
        os.tempStream << tabs(os.indentation);
        return os;
    }

    inline ApiStreamBuffer &dot(ApiStreamBuffer &os)
    {
        os.tempStream << ".";
        return os;
    }

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const tabs &obj)
    {
        size_t n = obj.getn();
        for (size_t i = 0; i < n; ++i)
            os.tempStream << "\t";
        return os;
    }

    inline ApiStreamBuffer &operator<<(ApiStreamBuffer &os, const tabs &obj)
    {
        size_t n = obj.getn();
        for (size_t i = 0; i < n; ++i)
            os.tempStream << "\t";
        return os;
    }

    inline std::ostringstream &operator<<(std::ostringstream &os, const tabs &obj)
    {
        size_t n = obj.getn();
        for (size_t i = 0; i < n; ++i)
            os << "\t";
        return os;
    }

    inline std::ostream &operator<<(ostream &os, const tabs &obj)
    {
        size_t n = obj.getn();
        for (size_t i = 0; i < n; ++i)
            os << "\t";
        return os;
    }

    class temp_name
    {
        size_t _n;

      public:
        explicit temp_name(size_t n)
            : _n(n)
        {
        }
        size_t getn() const { return _n; }
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const temp_name &obj)
        {
            os.tempStream << "obj" << obj.getn();
            return os;
        }
        friend std::ostringstream &operator<<(std::ostringstream &os, const temp_name &obj)
        {
            os << "obj" << obj.getn();
            return os;
        }
    };

    class clean_name
    {
        std::string _str;

      public:
        explicit clean_name(std::string &str)
            : _str(str)
        {
        }
        explicit clean_name(const char *str)
            : _str(str)
        {
        }
        string getstr() const { return _str; }
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const clean_name &obj);
        friend std::ostringstream &operator<<(std::ostringstream &os, const clean_name &obj);
        template < class _Traits >
        friend basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const clean_name &obj);
    };

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const clean_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
        }
        os.tempStream << newStr;
        return os;
    }
    inline std::ostringstream &operator<<(std::ostringstream &os, const clean_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
        }
        os << newStr;
        return os;
    }

    template < class _Traits >
    inline basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const clean_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
        }
        _Ostr << newStr;
        return _Ostr;
    }

    class var_name
    {
        std::string _str;

      public:
        explicit var_name(std::string &str)
            : _str(str)
        {
        }
        explicit var_name(const std::string &str)
            : _str(str)
        {
        }
        explicit var_name(const char *str)
            : _str(str)
        {
        }
        string getstr() const { return _str; }
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const var_name &obj);
        friend std::ostringstream &operator<<(std::ostringstream &os, const var_name &obj);
        template < class _Traits >
        friend basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const var_name &obj);
    };

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const var_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::tolower);
        }
        os.tempStream << newStr;
        return os;
    }

    inline std::ostringstream &operator<<(std::ostringstream &os, const var_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::tolower);
        }
        os << newStr;
        return os;
    }

    template < class _Traits >
    inline basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const var_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::tolower);
        }
        _Ostr << newStr;
        return _Ostr;
    }

    class type_name
    {
        std::string _str;

      public:
        explicit type_name(std::string &str)
            : _str(str)
        {
        }
        explicit type_name(const std::string &str)
            : _str(str)
        {
        }
        explicit type_name(const char *str)
            : _str(str)
        {
        }
        string getstr() const { return _str; }
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const type_name &obj);
        friend std::ostringstream &operator<<(std::ostringstream &os, const type_name &obj);
        template < class _Traits >
        friend basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const type_name &obj);
    };

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const type_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            if (!os.tempStream.str().length())
                newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::toupper);
        }
        os.tempStream << newStr;
        return os;
    }

    inline std::ostringstream &operator<<(std::ostringstream &os, const type_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            if (!os.str().length())
                newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::toupper);
        }
        os << newStr;
        return os;
    }

    template < class _Traits >
    inline basic_ostream< char, _Traits > &operator<<(basic_ostream< char, _Traits > &_Ostr, const type_name &obj)
    {
        string newStr = obj.getstr();
        if (newStr.length())
        {
            if (!_Ostr.width())
                newStr.erase(std::remove(newStr.begin(), newStr.end(), ':'), newStr.end());
            std::transform(newStr.begin(), newStr.begin() + 1, newStr.begin(), ::toupper);
        }
        _Ostr << newStr;
        return _Ostr;
    }

    class comment
    {
        std::string _str;

      public:
        explicit comment(std::string &str)
            : _str(str)
        {
        }
        explicit comment(const std::string &str)
            : _str(str)
        {
        }
        explicit comment(const char *str)
            : _str(str)
        {
        }
        string getstr() const { return _str; }
        friend ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const comment &obj);
    };

    inline ApiFileOutputStream &operator<<(ApiFileOutputStream &os, const comment &obj)
    {
        string commentStr = obj.getstr();
        if (commentStr.length())
        {
            string buffer = commentStr;
            std::size_t found = buffer.find("\n");
            while (found != std::string::npos)
            {
                string temp = buffer.substr(0, found);
                os.tempStream << "// ";
                os.tempStream << temp;
                os << endl;
                if (found + 1 < buffer.length())
                {
                    buffer = buffer.substr(found + 1, buffer.length() - (found + 1));
                }
                found = buffer.find("\n");
            }
            if (buffer.length())
            {
                os.tempStream << "// ";
                os.tempStream << buffer;
                os << endl;
            }
        }
        return os;
    }

    class expansionstringstream
    {
      public:
        enum Type
        {
            ARRAY = 0,
            OBJECT,
            MAP,
            VAR,
            NUM_TYPES
        };

        struct TypePair
        {
            Type type;
            std::string name;
        };

        expansionstringstream() {}

        expansionstringstream(const expansionstringstream &other)
        {
            varNameStream << other.varNameStream.str();
            typeNameStream << other.typeNameStream.str();
            prefixStream << other.prefixStream.str();
            typeStack = other.typeStack;
        }

        const char *getTypeString(Type type)
        {
            static const char *TypeString[] = {"Array", "Object", "Map", ""};
            return TypeString[(Int32)type];
        }

        void updateStack(const char *text)
        {
            string str(text);
            for (UInt32 typeIdx = 0; typeIdx < NUM_TYPES; typeIdx++)
            {
                const char *testString = getTypeString((Type)typeIdx);
                std::size_t found = str.find(testString);
                if (found != std::string::npos)
                {
                    TypePair tPair;
                    tPair.type = (Type)typeIdx;
                    tPair.name = text;
                    typeStack.push_back(tPair);
                    break;
                }
            }
        }

        friend expansionstringstream &operator<<(expansionstringstream &os, const char *text);
        friend expansionstringstream &dot(expansionstringstream &os);
        friend expansionstringstream &operator<<(expansionstringstream &os, string &str);
        friend expansionstringstream &operator<<(expansionstringstream &os, const string &str);
        friend expansionstringstream &operator<<(expansionstringstream &os, expansionstringstream::Type type);
        friend expansionstringstream &operator<<(expansionstringstream &os,
                                                 expansionstringstream &(*_Pfn)(expansionstringstream &));

        std::string prefix_str() { return prefixStream.str(); }

        std::string var_name_str() { return varNameStream.str(); }

        std::string type_name_str() { return typeNameStream.str(); }

        std::string debug_str()
        {
            std::ostringstream tempStream;
            std::vector< TypePair >::reverse_iterator rit = typeStack.rbegin();
            tempStream << "/*Take 1" << std::endl;
            for (; rit != typeStack.rend(); ++rit)
            {
                tempStream << "." << var_name(rit->name);
            }
            tempStream << std::endl;
            for (; rit != typeStack.rend(); ++rit)
            {
                tempStream << var_name(rit->name);
            }
            tempStream << std::endl;
            tempStream << "*/" << std::endl;
            return typeNameStream.str();
        }

        std::string debug_str_2()
        {
            std::string temp;

            bool arrayMapEncountered = false;
            std::vector< TypePair >::reverse_iterator rit = typeStack.rbegin();
            for (; rit != typeStack.rend(); ++rit)
            {
                std::vector< TypePair >::reverse_iterator previous = rit + 1;
                bool previousArrayOrMap = false;
                if (previous != typeStack.rend())
                {
                    if (previous->type == ARRAY || previous->type == MAP)
                    {
                        previousArrayOrMap = true;
                    }
                }
                std::ostringstream tempStr;
                if (arrayMapEncountered)
                {
                    tempStr << type_name(rit->name) << temp;
                    temp = tempStr.str();
                }
                else if (rit->type == VAR)
                {
                    tempStr << var_name(rit->name);
                    temp = tempStr.str();
                }
                else if (rit->type == MAP || rit->type == ARRAY)
                {
                    if (temp.length() && !previousArrayOrMap)
                    {
                        tempStr << type_name(rit->name) << std::dot << temp;
                    }
                    else
                    {
                        tempStr << type_name(rit->name);
                    }
                    temp = tempStr.str();
                    arrayMapEncountered = true;
                }
                else
                {
                    if (rit + 1 == typeStack.rend())
                    {
                        tempStr << var_name(rit->name) << std::dot << temp;
                        temp = tempStr.str();
                    }
                }
            }
            std::ostringstream tempStr;
            tempStr << var_name(temp);
            temp = tempStr.str();
            return temp;
        }

        std::string debug_stack_str()
        {
            std::ostringstream tempStr;
            std::vector< TypePair >::iterator it = typeStack.begin();
            for (; it != typeStack.end(); ++it)
            {
                tempStr << getTypeString(it->type) << ".";
            }
            return tempStr.str();
        }

        bool wasPreviousPrevious(Type type)
        {
            if (typeStack.size() > 1)
            {
                auto &lastType = typeStack[typeStack.size() - 2];
                return (lastType.type == type ? true : false);
            }
            return false;
        }

        bool wasPrevious(Type type)
        {
            if (typeStack.size())
            {
                auto &lastType = typeStack.back();
                return (lastType.type == type ? true : false);
            }
            return false;
        }

        bool has(Type type)
        {
            struct special_compare : public std::unary_function< Type, bool >
            {
                explicit special_compare(const Type &baseline)
                    : baseline(baseline)
                {
                }
                bool operator()(const TypePair &arg) { return arg.type == baseline; }
                Type baseline;
            };

            std::vector< TypePair >::iterator it = find_if(typeStack.begin(), typeStack.end(), special_compare(type));
            return (it != typeStack.end() ? true : false);
        }

        ostringstream varNameStream;
        ostringstream typeNameStream;
        ostringstream prefixStream;
        vector< TypePair > typeStack;
    };

    inline expansionstringstream &operator<<(expansionstringstream &os, const char *text)
    {
        bool previousWasArray =
            os.wasPrevious(expansionstringstream::ARRAY) || os.wasPrevious(expansionstringstream::MAP);
        os.updateStack(text);
        bool newIsObject = os.wasPrevious(expansionstringstream::OBJECT);
        if (!previousWasArray || !newIsObject)
        {
            if (!os.varNameStream.str().length())
            {
                os.varNameStream << var_name(text);
            }
            else
            {
                os.varNameStream << clean_name(text);
            }
            if (!os.typeNameStream.str().length())
            {
                os.typeNameStream << type_name(text);
            }
            else
            {
                os.typeNameStream << text;
            }
            if (!os.prefixStream.str().length())
            {
                os.prefixStream << var_name(text);
            }
            else
            {
                os.prefixStream << std::dot << var_name(text);
            }
        }
        return os;
    }

    inline expansionstringstream &dot(expansionstringstream &os)
    {
        if (os.prefixStream.str().length())
        {
            os.prefixStream << ".";
        }
        return os;
    }

    inline expansionstringstream &operator<<(expansionstringstream &os, string &str)
    {
        os << str.c_str();
        return os;
    }

    inline expansionstringstream &operator<<(expansionstringstream &os, const string &str)
    {
        os << str.c_str();
        return os;
    }

    inline expansionstringstream &operator<<(expansionstringstream &os, expansionstringstream::Type type)
    {
        os << os.getTypeString(type);
        return os;
    }

    inline expansionstringstream &operator<<(expansionstringstream &os,
                                             expansionstringstream &(*_Pfn)(expansionstringstream &))
    {
        return ((*_Pfn)(os));
    }

} // namespace trader
