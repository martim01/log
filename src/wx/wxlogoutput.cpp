#include "wx/wxlogoutput.h"

wxDEFINE_EVENT(wxEVT_LOG, wxCommandEvent);


wxLogOutput::wxLogOutput(wxEvtHandler* pHandler, bool bMilliseconds) : LogOutput(pml::LogOutput::TS_DATE | pml::LogOutput::TS_TIME,bMilliseconds ? pml::LogOutput::TSR_MILLISECOND : pml::LogOutput::TSR_SECOND),
m_pHandler(pHandler)
{
}


void wxLogOutput::Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream, const std::string& sPrefix)
{
    if(m_pHandler && eLogLevel >= m_eLevel)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_LOG);
        pEvent->SetTimestamp(wxDateTime::Now().GetTicks());
        pEvent->SetExtraLong(wxDateTime::UNow().GetMillisecond());
        pEvent->SetInt(eLogLevel);
        pEvent->SetString("["+wxString(sPrefix)+"] "+wxString(logStream.str()));

        wxQueueEvent(m_pHandler, pEvent);
    }
}



