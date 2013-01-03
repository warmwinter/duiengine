/***********************************************************************
    filename:   duiDefaultLogger.cpp
    created:    25/1/2006
    author:     Andrew Zabolotny

    purpose:    Implementation of the DefaultLogger class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The DuiEngine Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   _T("Software")), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED _T("AS IS"), WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "duistd.h"
#include "DuiDefaultLogger.h"
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include <tchar.h>


// Start of DuiEngine namespace section
namespace DuiEngine
{

    /*************************************************************************
        Constructor
    *************************************************************************/
    DefaultLogger::DefaultLogger(void) :
            d_caching(true),d_pFile(NULL)
    {
        // create log header
        logEvent(_T("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"));
        logEvent(_T("+                     DuiEngine - Event log                                   +"));
        logEvent(_T("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n"));
        TCHAR addr_buff[100];
		_stprintf(addr_buff, _T("DuiEngine::Logger singleton created: (%p)"), static_cast<void*>(this));
		logEvent(addr_buff);
    }

    /*************************************************************************
        Destructor
    *************************************************************************/
    DefaultLogger::~DefaultLogger(void)
    {
        if (d_pFile)
        {
            TCHAR addr_buff[100];
			_stprintf(addr_buff, _T("DuiEngine::Logger singleton destroyed.:(%p)"), static_cast<void*>(this));
            logEvent(addr_buff);
			fclose(d_pFile);
        }
    }

    /*************************************************************************
        Logs an event
    *************************************************************************/
    void DefaultLogger::logEvent(LPCTSTR message, LoggingLevel level /* = Standard */)
    {
        time_t  et;
        time(&et);
        tm* etm = localtime(&et);

        if (etm)
        {
            // clear sting stream
			CString strbuf;

			strbuf.Format(_T("%04/%02/02 %02:%02:%02 "),etm->tm_year,etm->tm_mon,etm->tm_mday,etm->tm_hour,etm->tm_min,etm->tm_sec);

            // write event type code
            switch(level)
            {
            case Errors:
                strbuf+= _T("(Error)\t");
                break;

            case Warnings:
                strbuf+= _T("(Warn)\t");
                break;

            case Standard:
                strbuf+= _T("(Std) \t");
                break;

            case Informative:
                strbuf+= _T("(Info) \t");
                break;

            case Insane:
                strbuf+= _T("(Insan)\t");
                break;

            default:
                strbuf+= _T("(Unkwn)\t");
                break;
            }

            strbuf+= message;
			strbuf+=_T("\n");

            if (d_caching)
            {
                d_cache.push_back(STL_NS::make_pair(strbuf, level));
            }
            else if (d_level >= level)
            {
                // write message
				_ftprintf(d_pFile,(LPCTSTR)strbuf);
				fflush(d_pFile);
            }
        }
    }


    void DefaultLogger::setLogFilename(LPCTSTR filename, bool append)
    {
		if(d_pFile)
		{
			fclose(d_pFile);
			d_pFile=NULL;
		}
		d_pFile=_tfopen(filename,append?_T("a+"):_T("w"));


        // write out cached log strings.
        if (d_caching)
        {
            d_caching = false;

            STL_NS::vector<STL_NS::pair<CString, LoggingLevel> >::iterator iter = d_cache.begin();

            while (iter != d_cache.end())
            {
                if (d_level >= (*iter).second)
                {
					_ftprintf(d_pFile,(LPCTSTR)(*iter).first);
					fflush(d_pFile);
                }

                ++iter;
            }

            d_cache.clear();
        }
    }

} // End of  DuiEngine namespace section
