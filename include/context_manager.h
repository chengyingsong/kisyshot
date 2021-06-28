#pragma once

#include "context.h"
namespace kisyshot {
    /**
     * A Context manager: which is going to be used in multiple file compilations.
     */
    class ContextManager {
    public:
        /**
         * Create a SyntaxContext by given code reference string
         * @param code
         * @return
         */
        std::shared_ptr<Context> create(const std::string_view &code);

        /**
         * Access to the SyntaxContext with the given index
         * @param index
         * @return
         */
        std::shared_ptr<Context> access(std::size_t index);

        /**
         * Access to the SyntaxContext with the given index
         * @param index
         * @return
         */
        std::shared_ptr<Context> operator[](std::size_t index);

    private:
        std::vector<std::shared_ptr<Context>> _contexts;
    };
}