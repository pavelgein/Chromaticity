#include "writer.h"

class TWriterTask : public ITask {
public:
    explicit TWriterTask(std::ostream& out, std::string&& s)
        : Out(out)
        , S(std::move(s))
    {
    }

    void Do() override {
        Out << S;
    }

private:
    std::ostream& Out;
    std::string S;
};

void TWriter::Push(std::string&& tokens) {
    Executer.Add(std::make_unique<TWriterTask>(Out, std::move(tokens)));
}

TWriter::~TWriter() {

}
