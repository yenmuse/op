#pragma once
#ifndef __DICT_H_
#define __DICT_H_
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include "bitfunc.h"
//#define SET_BIT(x, idx) x |= 1u << (idx)

//#define GET_BIT(x, idx) ((x >> (idx)) & 1u)
#define WORD_COLOR 0
#define WORD_BKCOLOR 255
struct rect_t {
	int x1, y1;
	int x2, y2;
	int width() const { return x2 - x1; }
	int height() const { return y2 - y1; }
};
struct word_t {

	struct word_info_t {
		//char of word
		wchar_t _char[4];
		//char height
		__int16 width, height;
		//char bit ct
		__int32 bit_count;
		word_info_t() :width(0), height(0), bit_count(0) { memset(_char, 0, sizeof(_char)); }
		bool operator==(const word_info_t& rhs) {
			return width == rhs.width&&height == rhs.height;
		}
		bool operator!=(const word_info_t& rhs) {
			return width != rhs.width&&height == rhs.height;
		}

	};
	//32 bit a col
	using cline_t = unsigned __int32;
	word_info_t info;
	//char col line
	cline_t clines[32];
	bool operator==(const word_t& rhs) {
		if (info != rhs.info)
			return false;
		for (int i = 0; i < info.width; ++i)
			if (clines[i] != rhs.clines[i])
				return false;
		return true;
	}
	void set_chars(const std::wstring&s) {
		memcpy(info._char, s.c_str(), std::min(sizeof(info._char), (s.length() + 1) * sizeof(wchar_t)));
	}
};

struct Dict {
	struct dict_info_t {
		__int16 _this_ver;
		__int16 _word_count;
		//check code=_this_ver^_word_count
		__int32 _check_code;
		dict_info_t() :_this_ver(0), _word_count(0) { _check_code = _word_count ^ _this_ver; }
	};
	dict_info_t info;
	Dict() {}
	std::vector<word_t>words;
	void read_dict(const std::wstring&s) {
		std::fstream file;
		file.open(s, std::ios::in | std::ios::binary);
		if (!file.is_open())
			return;
		//读取信息
		file.read((char*)&info, sizeof(info));
		//校验
		if (info._check_code == (info._this_ver^info._word_count)) {
			words.resize(info._word_count);
			file.read((char*)&words[0], sizeof(word_t)*info._word_count);
		}
		file.close();
	}
	void write_dict(const std::string&s) {
		std::fstream file;
		file.open(s, std::ios::out | std::ios::binary);
		if (!file.is_open())
			return;
		//设置校验
		info._check_code = info._this_ver^info._word_count;
		//写入信息
		file.write((char*)&info, sizeof(info));
		//写入数据
		file.write((char*)&words[0], sizeof(word_t)*info._word_count);
		file.close();
	}
	void add_word(const cv::Mat& binary, const rect_t& rc) {
		int x2 = std::min(rc.x1 + 32, rc.x2);
		int y2 = std::min(rc.y1 + 32, rc.y2);
		word_t word;
		for (int j = rc.x1; j < x2; ++j) {
			unsigned __int32 x = 0, val;
			for (int i = rc.y1, id = 0; i < y2; ++i, ++id) {
				val = binary.at<uchar>(i, j);
				if (val == 0)
					SET_BIT(x, 31 - id);
			}
			word.clines[word.info.width++] = x;
			//t = QString::asprintf("%08X", x);
			//tp += t;

		}
		word.info.height = y2 - rc.y1;
		auto it = find(word);
		if (words.empty() || it == words.end()) {
			word.set_chars(L"");

			words.push_back(word);
			info._word_count = words.size();
		}
		else {//only change char
			//word.set_chars(c);
		}

	}
	void add_word(const word_t&word) {
		auto it = find(word);
		if (words.empty() || it == words.end()) {
			words.push_back(word);
		}
		else {
			it->set_chars(word.info._char);
		}
		info._word_count = words.size();
	}
	void clear() {
		info._word_count = 0;
		words.clear();
	}
	std::vector<word_t>::iterator find(const word_t&word) {
		for (auto it = words.begin(); it != words.end(); ++it)
			if (*it == word)return it;
		return words.end();
	}

	void erase(const word_t&word) {
		auto it = find(word);
		if (!words.empty() && it != words.end())
			words.erase(it);
		info._word_count = words.size();
	}
};

#endif
