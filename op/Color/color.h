#pragma once
#ifndef __COLOR_H_
#define __COLOR_H_
#include <string>
#include <opencv2/core.hpp>
#include <boost/format.hpp>
#include "../Common.h"
//颜色结构
struct color_t {
	uchar b, g, r, alpha;
	color_t() :b(0), g(0), r(0), alpha(0) {}
	//absolute val
	color_t operator-(const color_t& rhs) {
		color_t c;
		c.b = b - rhs.b;
		c.g = g - rhs.g;
		c.r = r - rhs.r;
		return c;
	}
	bool operator<=(const color_t& rhs) {
		return b <= rhs.b&&g <= rhs.g&&r <= rhs.r;
	}
	color_t& str2color(std::wstring&s) {
		int r, g, b;
		std::transform(s.begin(), s.end(), s.begin(), ::towupper);
		swscanf(s.c_str(), L"%02X%02X%02X", &r, &g, &b);
		this->b = b; this->r = r; this->g = g;
		return *this;
	}
	std::wstring tostr() {
		char buff[10];
		sprintf(buff, "%02X%02X%02X", r, g, b);
		return _sto_wstring(buff);
	}
};

//颜色-偏色结构
struct color_df_t {
	//颜色
	color_t color;
	//偏色
	color_t df;
};
//坐标-颜色-偏色结构
struct pt_cr_df_t {
	int x, y;
	std::vector<color_df_t> crdfs;
};

#endif
