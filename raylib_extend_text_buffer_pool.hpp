/*
    @author yuan
    @brief a raylib extend tool, to solve the Chinese characters shown problems, written in C++17, only one header file.
*/
#pragma once

#include <string>
#include <stdexcept>
#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <raylib.h>

namespace raylib_extend {
    class FontNotFoundException : public std::runtime_error {
    public:
        FontNotFoundException(const std::string& fontPath)
            : std::runtime_error{ "load font file failed: " + fontPath }
        {}
    };

    struct TextData {
        std::string text;
        Font font;
        int fontSize;
    };

    class TextPool {
        unsigned char* fontFileData;
        int dataSize;
        std::unordered_map<std::string_view, std::shared_ptr<TextData>> pool;

        void init_font(Font& font, int fontSize, std::string_view text) {
            int codepointsCount;
            int* codepoints = LoadCodepoints(text.data(), &codepointsCount);
            font = LoadFontFromMemory(".ttf", fontFileData, dataSize, fontSize, codepoints, codepointsCount);
            UnloadCodepoints(codepoints);
        }

        TextPool() 
            : fontFileData{ nullptr }, dataSize{ 0 }, pool{}
        {}
    public:
        static TextPool& instance() {
            static TextPool pool;
            return pool;
        }

        ~TextPool() {
            for (auto& [k, v] : pool) {
                UnloadFont(v->font);
            }

            if (fontFileData) {
                UnloadFileData(fontFileData);
            }
        }

        TextPool(const TextPool&) = delete;
        TextPool& operator=(const TextPool&) = delete;
        TextPool(TextPool&& other) = delete;
        TextPool& operator=(TextPool&& other) = delete;

        void load_font(const std::string& fontPath) {
            if (!std::filesystem::exists(fontPath)) {
                throw FontNotFoundException{ fontPath };
            }

            if (fontFileData) {
                UnloadFileData(fontFileData);
            }

            fontFileData = LoadFileData(fontPath.c_str(), &dataSize);
        }

        void put(std::string_view text, int fontSize) {
            auto iter = pool.find(text);

            if (iter != pool.cend()) {
                if (iter->second->fontSize != fontSize) {
                    iter->second->fontSize = fontSize;
                    UnloadFont(iter->second->font);
                    init_font(iter->second->font, fontSize, text);
                }
            }
            else {
                auto td = std::make_shared<TextData>();
                td->fontSize = fontSize;
                td->text = text;
                init_font(td->font, fontSize, text);

                pool.emplace(td->text, td);
            }

            // never return the iterator to the caller, because rehash may happens.
        }

        const std::shared_ptr<TextData> get(std::string_view text) {
            auto iter = pool.find(text);
            return iter != pool.cend() ? iter->second : nullptr;
        }

        void del(std::string_view text) {
            pool.erase(text);
        }

        const std::unordered_map<std::string_view, std::shared_ptr<TextData>>& seek_pool() const noexcept {
            return pool;
        }

        const std::unordered_map<std::string_view, std::shared_ptr<TextData>>::size_type size() const noexcept {
            return pool.size();
        }
    };
}
