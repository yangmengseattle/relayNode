/*
 * utils.h
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
using namespace std;

string getLocalIpAddress();
vector<string> split(const string& text, const string& delims);
void printHex(char* arr);

#endif /* UTILS_H_ */
