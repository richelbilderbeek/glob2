/*
  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at nct@ysagoon.com or nuage@ysagoon.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __STRINGTABLE_H
#define __STRINGTABLE_H

#include <vector>
#include <map>
#include <string>

namespace GAGCore
{
	class OneStringToken
	{
	public:
		std::vector<std::string> data;
	};
	
	class StringTable
	{
	public:
		StringTable();
		~StringTable();
		void setLang(int l) { actLang = l; }
		void setDefaultLang(int l) { defaultLang = l; }
		int getLang(void) { return actLang; }
		int getNumberOfLanguage(void) { return languageCount; }
		bool load(const char *filename);
		std::string getString(const std::string &stringname, int index = -1) const;
		const char *getString(const char *stringname, int index = -1) const;
		std::string getStringInLang(const std::string &stringname, int lang) const;
		const char *getStringInLang(const char *stringname, int lang) const;
		void print();
	
	private:
		std::vector<OneStringToken *> strings;
		std::map<std::string, size_t> stringAccess;
		int actLang;
		int defaultLang;
		int languageCount;
		
	public:
		enum {AI_NAME_SIZE=4};
	};

	//! Translate to current language
	std::string tr(const std::string &key);
}

#endif

