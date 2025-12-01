/*
    @author yuan
    @brief a raylib extend tool, to solve the Chinese characters shown problems, written in C++17, only one header file.
*/
#pragma once

#include <string>
#include <stdexcept>
#include <filesystem>
#include <utility>
#include <raylib.h>

namespace raylib_extend {
    class FontNotFoundException : public std::runtime_error {
    public:
        FontNotFoundException(const std::string& fontPath)
            : std::runtime_error{ "load font file failed: " + fontPath }
        {}
    };

    // a wrapper for raylib font file data.
    class FontFileData {
        unsigned char* data;
        int dataSize;
    public:
        FontFileData() : data{ nullptr }, dataSize{ 0 } {}

        FontFileData(const std::string& fontPath) : data { nullptr }, dataSize{ 0 } {
            reset(fontPath);
        }

        ~FontFileData() {
            if (data) {
                UnloadFileData(data);
            }
        }

        FontFileData(const FontFileData&) = delete;
        FontFileData& operator=(const FontFileData&) = delete;

        FontFileData(FontFileData&& other) noexcept 
            : data{ std::exchange(other.data, nullptr) }, dataSize{ std::exchange(other.dataSize, 0) } 
        {}

        FontFileData& operator=(FontFileData&& other) noexcept {
            if (this != &other) {
                if (data) {
                    UnloadFileData(data);
                }
                
                data = std::exchange(other.data, nullptr);
                dataSize = std::exchange(other.dataSize, 0);
            }

            return *this;
        }

        void reset(const std::string& fontPath) {
            if (!std::filesystem::exists(fontPath)) {
                throw FontNotFoundException{ fontPath };
            }

            if (data) {
                UnloadFileData(data);
            }

            data = LoadFileData(fontPath.c_str(), &dataSize);
        }

        const unsigned char* get_data() const noexcept { 
            return data; 
        }

        int get_data_size() const noexcept { 
            return dataSize; 
        }
    };

    // a wrapper for raylib codepints.
    class CodePoints {
        int* cp;
        int cpCount;
    public:
        CodePoints(const std::string& text) : cp{ nullptr }, cpCount{ 0 } {
            cp = LoadCodepoints(text.data(), &cpCount);
        }

        ~CodePoints() {
            if (cp) {
                UnloadCodepoints(cp);
            }
        }

        CodePoints(const CodePoints&) = delete;
        CodePoints& operator=(const CodePoints&) = delete;

        CodePoints(CodePoints&& other) noexcept 
            : cp{ std::exchange(other.cp, nullptr) }, cpCount{ std::exchange(other.cpCount, 0) } 
        {}

        CodePoints& operator=(CodePoints&& other) noexcept {
            if (this != &other) {
                if (cp) {
                    UnloadCodepoints(cp);
                }
                
                cp = std::exchange(other.cp, nullptr);
                cpCount = std::exchange(other.cpCount, 0);
            }

            return *this;
        }

        int* get_data() const noexcept {
            return cp;
        }

        int get_count() const noexcept {
            return cpCount;
        }
    };

    class TextData {
        std::string _text;
        Font _font;
        int _fontSize;
        bool loaded;
    public:
        TextData(const std::string& text, const FontFileData& ffd, int fontSize) {
            reset(text, ffd, fontSize);
            loaded = true;
        }

        ~TextData() {
            if (loaded) {
                UnloadFont(_font);
            }
        }

        TextData(const TextData&) = delete;
        TextData& operator=(const TextData&) = delete;

        TextData(TextData&& other) noexcept
            : _text{ std::exchange(other._text, "") }, _font{ std::exchange(other._font, {}) }, _fontSize{ std::exchange(other._fontSize, 0) }, loaded{ std::exchange(other.loaded, false) }
        {}

        TextData& operator=(TextData&& other) noexcept {
            if (this != &other) {
                if (loaded) {
                    UnloadFont(_font);
                }

                _text = std::exchange(other._text, "");
                _font = std::exchange(other._font, {});
                _fontSize = std::exchange(other._fontSize, 0);
                loaded = std::exchange(other.loaded, false);
            }

            return *this;
        }

        void reset(const std::string& text, const FontFileData& ffd, int fontSize) {
            _text = text;
            _fontSize = fontSize;

            CodePoints cp{ _text };
            _font = LoadFontFromMemory(".ttf", ffd.get_data(), ffd.get_data_size(), _fontSize, cp.get_data(), cp.get_count());
        }

        const std::string& text() const noexcept {
            return _text;
        }

        const Font& font() const noexcept {
            return _font;
        }

        int font_size() const noexcept {
            return _fontSize;
        }
    };
}
