#include "wx/wxlogoutput.h"

wxDEFINE_EVENT(wxEVT_LOG, wxCommandEvent);


wxLogOutput::wxLogOutput(wxEvtHandler* pHandler, bool bMilliseconds) : LogOutput(pml::LogOutput::TS_DATE | pml::LogOutput::TS_TIME,bMilliseconds ? pml::LogOutput::TSR_MILLISECOND : pml::LogOutput::TSR_SECOND),
m_pHandler(pHandler)
{
}


void wxLogOutput::Flush(pml::log::Level level, const std::string&  sLog, const std::string& sPrefix)
{
    if(m_pHandler && level >= m_level)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_LOG);
        pEvent->SetTimestamp(wxDateTime::Now().GetTicks());
        pEvent->SetExtraLong(wxDateTime::UNow().GetMillisecond());
        pEvent->SetInt(static_cast<int>(level));
        pEvent->SetString("["+wxString(sPrefix)+"] "+wxString(sLog));

        wxQueueEvent(m_pHandler, pEvent);
    }
}



