#include <deque>
#include <exception>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


class TBadOptionException : public std::invalid_argument {
public:
    TBadOptionException(const char* msg)
        : std::invalid_argument(msg)
    {
    }

    TBadOptionException(const std::string& msg)
        : std::invalid_argument(msg)
    {
    }
};

void FromString(const char * option, int& k) {
    k = std::atoi(option);
}

void FromString(const char * option, unsigned int& k) {
    k = std::atol(option);
}

void FromString(const char * option, std::string& s) {
   s = std::string(option);
}

struct IHandler {
    virtual ~IHandler() = default;

    virtual void Handle(int argc, const char ** argv, int& optNum) = 0;

    virtual void HandleSingle(const char * option) = 0;
};


template<typename T>
class TSimpleHandler : public IHandler {
public:
    TSimpleHandler(T* target)
        : Target(target)
    {
    }

    void Handle(int argc, const char ** argv, int& optNum) override {
        if (argc <= optNum) {
            throw TBadOptionException("expected argument");
        }

        HandleSingle(argv[optNum]);
        ++optNum;
    }

    void HandleSingle(const char * option) override {
        FromString(option, *Target);
    }

private:
    T* Target;
};

template<typename T>
class TRepeatedHandler : public IHandler {
public:
    TRepeatedHandler(std::vector<T>* target)
        : Target(target)
    {
    }

    void Handle(int argc, const char ** argv, int& optNum) override {
        while (optNum < argc) {
            const char * currentOption = argv[optNum];
            if (currentOption[0] == '-') {
                break;
            }

            HandleSingle(currentOption);
            ++optNum;
        }
    }

    void HandleSingle(const char * option) override {
        T value;
        FromString(option, value);
        Target->push_back(value);
    }

private:
    std::vector<T>* Target;
};


struct TOpt {
    std::string Name;
    std::unique_ptr<IHandler> Handler;
    bool Required_ = false;
    bool Used = false;
    std::optional<std::string> Default_;

    TOpt(char name)
        : Name{1, name}
    {
    }

    TOpt(std::string name)
        : Name(name)
    {
    }

    TOpt& Required(bool value) {
        Required_ = value;
        return *this;
    }

    TOpt& Default(const std::string& value) {
        Default_ = value;
        return *this;
    }

    template<typename T>
    TOpt& Store(T* target) {
        Handler = std::make_unique<TSimpleHandler<T>>(target);
        return *this;
    }

    template<typename T>
    TOpt& AppendTo(std::vector<T>* target) {
        Handler = std::make_unique<TRepeatedHandler<T>>(target);
        return *this;
    }

    void Handle(int argc, const char ** argv, int& optNum) {
        Used = true;
        IHandler* raw = Handler.get();
        if (raw == nullptr) {
            throw TBadOptionException("No handler for me!");
        }

        Handler->Handle(argc, argv, optNum);
    }

    bool HasDefault() const {
        return Default_.has_value();
    }

    void UseDefault() {
        Used = true;
        Handler->HandleSingle(Default_.value().c_str());
    }

};

struct TParser {
public:
    TOpt& AddLongOption(char s, std::string longName) {
        if (auto shortIter = ShortOptions.find(s); shortIter != ShortOptions.end()) {
            throw TBadOptionException("duplicate short option");
        }

        if (auto longIter = LongOptions.find(longName); longIter != LongOptions.end()) {
            throw TBadOptionException("duplicate long option");
        }

        Options.emplace_back(longName);
        ShortOptions.emplace(s, &Options.back());
        LongOptions.emplace(longName, &Options.back());

        return Options.back();
    }

    TOpt& AddLongOption(std::string longName) {
        if (auto longIter = LongOptions.find(longName); longIter != LongOptions.end()) {
            throw TBadOptionException("duplicate long option");
        }

        Options.emplace_back(longName);
        LongOptions.emplace(longName, &Options.back());

        return Options.back();
    }

    TOpt& AddShortOption(char shortName) {
        if (auto shortIter = ShortOptions.find(shortName); shortIter != ShortOptions.end()) {
            throw TBadOptionException("duplicate short option");
        }

        Options.emplace_back(shortName);
        ShortOptions.emplace(shortName, &Options.back());

        return Options.back();
    }

    TOpt& AddFreeArgument(const std::string& name) {
        Options.emplace_back(name);
        FreeArguments.emplace_back(&Options.back());
        return Options.back();
    }

    void Parse(int argc, const char ** argv) {
        int optNum = 1;
        size_t freeArgNum = 0;
        while (optNum < argc) {
            const char * currentOption = argv[optNum];
            if (currentOption[0] != '-') {
                HandleFreeArg(argc, argv, optNum, freeArgNum);
                ++freeArgNum;
            } else {
                if (strlen(currentOption) < 2) {
                    throw TBadOptionException("expected option name");
                }

                if (currentOption[1] == '-') {
                    HandleLongOption(argc, argv, optNum);
                } else {
                    HandleShortOption(argc, argv, optNum);
                }
            }
        }

        Check();
    }

private:
    void Check() {
        for (auto& option : Options) {
            if (!option.Used) {
                if (option.HasDefault()) {
                    option.UseDefault();
                } else if (option.Required_) {
                    throw TBadOptionException((std::stringstream() << "option " << option.Name << " marked as required  and not set").str());

                }
            }
        }
    }

    void HandleFreeArg(int argc, const char ** argv, int& optNum, size_t freeArgNum) {
        if (freeArgNum >= FreeArguments.size()) {
            throw TBadOptionException("too much free arguments");
        }

        TOpt* opt = FreeArguments[freeArgNum];
        opt->Handle(argc, argv, optNum);
    }

    void HandleLongOption(int argc, const char ** argv, int& optNum) {
        const char * current = argv[optNum];
        const char * name = current + 2;
        if (auto iter = LongOptions.find(name); iter != LongOptions.end()) {
            ++optNum;
            iter->second->Handle(argc, argv, optNum);
        } else {
            throw TBadOptionException("unknown short option");
        }
    }

    void HandleShortOption(int argc, const char ** argv, int& optNum) {
        const char * current = argv[optNum];
        const char * name = current + 1;
        if (auto iter = ShortOptions.find(name[0]); iter != ShortOptions.end()) {
            ++optNum;
            iter->second->Handle(argc, argv, optNum);
        } else {
            throw TBadOptionException("unknown short option");
        }
    }

    std::deque<TOpt> Options;
    std::vector<TOpt*> FreeArguments;
    std::unordered_map<char, TOpt*> ShortOptions;
    std::unordered_map<std::string, TOpt*> LongOptions;
};

