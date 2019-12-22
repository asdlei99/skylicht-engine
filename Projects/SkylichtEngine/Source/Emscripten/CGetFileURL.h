/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#ifndef _EM_GETFILE_H_
#define _EM_GETFILE_H_

namespace Skylicht
{
	class CGetFileURL
	{
	public:
		enum EMethod
		{
			Get = 0,
			Post
		};

		enum EState
		{
			None,
			Downloading,
			Finish,
			Error
		};

	private:
		std::string m_params;
		std::string m_url;
		std::string m_fileName;

		EState m_state;
		int m_errorCode;

	public:
		CGetFileURL(const char *url, const char *fileName);

		~CGetFileURL();

		void setParams(const char *params)
		{
			m_params = params;
		}

		void download(EMethod method);

		void onLoad(unsigned int size, const char *path);

		void onStatus(unsigned int size, int status);

		void onProcess(unsigned int size, int percent);

		inline EState getState()
		{
			return m_state;
		}

		inline int getErrorCode()
		{
			return m_errorCode;
		}
	};
}

#endif