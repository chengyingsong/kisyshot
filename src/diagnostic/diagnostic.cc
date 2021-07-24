#include <cassert>
#include <diagnostic/diagnostic.h>
#include <iomanip>
#include <queue>
#include <stack>
namespace kisyshot::diagnostic {
    Diagnostic::Diagnostic(DiagnosticKind kind, std::shared_ptr<Context> context, std::string message):
        _kind(kind), _context(std::move(context)), _message(std::move(message)) {

    }

    Diagnostic &Diagnostic::at(std::size_t start, std::size_t finish, std::size_t caret, std::string label) {
        _locations.emplace_back(start, finish, caret, std::move(label));
        return *this;
    }

    Diagnostic &Diagnostic::at(std::size_t caret) {
        _locations.emplace_back(caret);
        return *this;
    }

    void Diagnostic::writeTo(std::ostream &stream) const {
        // we assume that a diagnostic can be wrote must have errors between (1, 3)
        // and the first diagnostic location is the critical diagnostic
        assert(!_locations.empty() && _locations.size() <=3);
        DiagnosticLocation criticalLocation = _locations[0];
        CodePosition criticalTokenLocation = _context->locate(criticalLocation.caret);
        std::size_t row = criticalTokenLocation.line;
        std::size_t column = criticalTokenLocation.offset;

        // print diagnostic title
        stream << _context->path << ":" << row << ":" << column << ": ";

        rang::fg color;
        switch (_kind) {
            case Error:
                stream << rang::style::bold << (color = rang::fg::red) << "error: ";
                break;
            case Note:
                stream << rang::style::bold << (color = rang::fg::cyan) << "note: ";
                break;
            case Warning:
                stream << rang::style::bold << (color = rang::fg::magenta) << "warning: ";
                break;
        }
        // show emphasize as bold
        if (!_emphasize.empty())
            stream << rang::fg::reset << "‘" << _emphasize << "’ ";
        stream << rang::fg::reset << rang::style::reset <<  _message << std::endl;

        _locations[0].color = color;
        if (_locations.size() >= 2)
            _locations[1].color = rang::fg::green;

        if (_locations.size() == 3)
            _locations[2].color = rang::fg::blue;

        std::sort(_locations.begin(), _locations.end(),
                  [](const DiagnosticLocation &l1, const DiagnosticLocation &l2) -> bool {
            return l1.start < l2.start;
        });

        size_t line = -1;
        // offset to the start of the line
        size_t offset = 0;
        size_t startToken = _locations[0].start;
        std::queue<std::tuple<int, int, int>> underlines;
        std::vector<std::tuple<int, int, int>> carets;

        auto handleUnderlines = [&underlines, &stream, this, color]()->void {
            stream << std::endl << std::setw(6) << "" << " |";
            int offset = 0;
            while (!underlines.empty()) {
                int s, len, idx;
                std::tie(s, len, idx) = underlines.front();
                stream << _locations[idx].color;
                stream << std::setw(s - offset) << std::setfill(' ') << "";
                stream << std::setw(offset == 0 ? len + 1 : len) << std::setfill('~')
                       << std::left << ((_locations[idx].color == color) ? "^" : "~");
                stream << std::setfill(' ') << std::right << rang::fg::reset;
                underlines.pop();
                offset = len + s;
            }
        };
        auto handleCarets = [&stream, &carets, this]() ->void {
            stream << std::endl << std::setw(6) << "" << " |";
            size_t offset = 0;
            for (auto & caret : carets) {
                size_t tokenId, tokenOffset, locationId;
                std::tie(tokenId, tokenOffset, locationId) = caret;
                stream << std::setw(tokenOffset - offset) << std::setfill(' ') << "";
                stream << _locations[locationId].color << "|";
                offset = tokenOffset + 1;
            }
            stream << rang::fg::reset;
            std::vector<std::stack<std::tuple<size_t , size_t, size_t>>> levelCarets;
            std::vector<std::tuple<size_t, size_t, size_t, size_t>> printCarets;
            levelCarets.push_back(std::stack<std::tuple<size_t, size_t, size_t>>());
            levelCarets[0].push(carets.back());
            printCarets.push_back(std::tuple_cat(carets.back(), std::make_tuple(0)));
            carets.pop_back();
            while (!carets.empty()){
                size_t level = 0;
                while (level < levelCarets.size()){
                    int tokenId, tokenOffset, locationId;
                    std::tie(tokenId, tokenOffset, locationId) = carets.back();
                    if (tokenOffset + _context->tokens[tokenId]->raw_code.size() < std::get<1>(levelCarets[level].top())){
                        goto done;
                    }
                }
                level++;
                levelCarets.push_back(std::stack<std::tuple<size_t, size_t, size_t>>());
                done:
                levelCarets[level].push(carets.back());
                printCarets.push_back(std::tuple_cat(carets.back(), std::make_tuple(level)));
                carets.pop_back();
            }
            std::reverse(printCarets.begin(), printCarets.end());
            for (size_t i = 0; i < levelCarets.size(); ++i) {
                stream << std::endl << std::setw(6) << "" << " |";
                offset = 0;
                for (size_t j = 0; j < printCarets.size(); ++j) {
                    size_t tokenId, tokenOffset, locationId, level;
                    std::tie(tokenId, tokenOffset, locationId, level) = printCarets[j];
                    if (level > i){
                        stream << std::setw(tokenOffset - offset) << std::setfill(' ') << "";
                        stream << _locations[locationId].color << "|" << rang::fg::reset;
                        offset = tokenOffset + 1;
                    }
                    if (level == i){
                        stream << std::setw(tokenOffset - offset) << std::setfill(' ') << "";
                        stream << _locations[locationId].color << _locations[locationId].label  << rang::fg::reset;
                        offset = tokenOffset + _locations[locationId].label.size();
                    }
                }
            }

        };
        for (size_t i = 0; i < _locations.size(); ++i) {
            offset = 0;
            CodePosition curPos = _context->locate(startToken);
            size_t curLine = curPos.line - 1;
            if (line != curLine) {
                if (!underlines.empty()) {
                    handleUnderlines();
                }
                if (!carets.empty()){
                    handleCarets();
                    stream << std::endl;
                }
                stream << std::setw(6) << curLine + 1<< " |";
                line = curLine;
            }
            // push non-colored text like in error (marked as '*'):
            //        t.c:7:39: error: invalid operands to binary expression ('int' and 'struct A')
            //              return y + func(y ? ((SomeA.X + 40) + SomeA) / 42 + SomeA.X : SomeA.X);
            //                                  ~~~~~~~~~~~~~~~*^ ~~~~~
            //                                            |       |
            //                                            int     S
            stream << _context->lines[curLine].substr(offset, curPos.offset );
            offset = curPos.offset;
            // init color
            for (; i < _locations.size(); ++i) {
                size_t finish = _context->lastOfLine(line);
                // push caret information
                if (_locations[i].caret <= finish && _locations[i].caret >= startToken && !_locations[i].label.empty()) {
                    carets.emplace_back(_locations[i].caret, _context->locate(_locations[i].caret).offset, i);
                }
                bool breaking = false;
                // determine whether colored output ends by '\n' or diagnostic ending
                if (_locations[i].finish < finish) {
                    // diagnostic ends first
                    finish = _locations[i].finish;
                } else{
                    breaking = true;
                }

                curPos = _context->locate(finish);
                size_t endingCharPos = _context->tokens[finish]->raw_code.size() + curPos.offset - 1;

                stream << _locations[i].color;
                stream << _context->lines[curLine].substr(offset, endingCharPos - offset + 1) << rang::fg::reset;
                underlines.emplace(offset, endingCharPos - offset, i);
                startToken = finish + 1;

                offset = curPos.offset + _context->tokens[finish]->raw_code.size();
                curPos = _context->locate(startToken);
                if (curPos.line - 1 != line)
                    break;

                if (breaking) {
                    --i;
                    break;
                }

            }
        }
        stream << _context->lines[line].substr(offset);
        if (!underlines.empty()) {
            handleUnderlines();
        }
        if (!carets.empty()){
            handleCarets();
        }
        stream << std::endl;
    }

    Diagnostic &Diagnostic::emphasize(std::string emphasize) {
        _emphasize = std::move(emphasize);
        return *this;
    }

    DiagnosticLocation::DiagnosticLocation(std::size_t start, std::size_t finish, std::size_t caret,
                                           std::string label) :
                                           start(start), finish(finish),caret(caret),label(std::move(label)){

    }

    DiagnosticLocation::DiagnosticLocation(std::size_t caret):
        start(caret), finish(caret),caret(caret),label() {

    }
}