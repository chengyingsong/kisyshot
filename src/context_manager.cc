#include <context_manager.h>

namespace kisyshot{
    std::shared_ptr<Context> ContextManager::create(const std::string_view& code) {
        auto context = std::make_shared<Context>(code, _contexts.size());
        _contexts.push_back(context);
        return context;
    }
    std::shared_ptr<Context> ContextManager::access(std::size_t index) {
        if(index < _contexts.size())
            return _contexts[index];
        return nullptr;
    }

    std::shared_ptr<Context> ContextManager::operator[](std::size_t index) {
        if(index < _contexts.size())
            return _contexts[index];
        return nullptr;
    }
}