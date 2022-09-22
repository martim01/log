#include "wxlogoutput.h"

wxDEFINE_EVENT(wxEVT_LOG, wxCommandEvent);


wxLogOutput::wxLogOutput(wxEvtHandler* pHandler, bool bMilliseconds) : LogOutput(pml::LogOutput::TS_DATE | pml::LogOutput::TS_TIME,bMilliseconds ? pml::LogOutput::TSR_MILLISECOND : pml::LogOutput::TSR_SECOND),
m_pHandler(pHandler)
{
}


void wxLogOutput::Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream)
{
    if(m_pHandler && eLogLevel >= m_eLevel)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_LOG);
        pEvent->SetTimestamp(wxDateTime::Now().GetTicks());
        pEvent->SetInt(eLogLevel);
        pEvent->SetString(wxString(logStream.str()));

//        wxString sMessage;
//        sMessage << wxString(Timestamp().str()) << "    " ;
//
//        wxString sLevel = "[" + pml::LogStream::STR_LEVEL[eLogLevel] + "]";
//
//        sMessage << sLevel;
//        for(size_t i = sLevel.length(); i < 12; i++)
//        {
//            sMessage << " ";
//        }
//         sMessage << wxString(logStream.str());
//
//        pEvent->SetString(sMessage.c_str());
        wxQueueEvent(m_pHandler, pEvent);
    }
}



