#include <iostream>

template<class TIter>
std::ostream& PrintRange(std::ostream& outp, TIter begin, TIter end) {
    while (begin != end) {
        outp << *begin <<  " ";
        ++begin;
    };

    return outp;
}

template<class TContainer>
std::ostream& PrintCollection(std::ostream& outp, const TContainer& container) {
    return PrintRange(outp, std::begin(container), std::end(container));
}


