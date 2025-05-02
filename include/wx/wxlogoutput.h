#pragma once
#include "log.h"
#include <string>
#include <fstream>
#include <wx/event.h>

class wxLogOutput : public pml::LogOutput
{
    public:
        explicit wxLogOutput(wxEvtHandler* pHandler, bool bMilliseconds=false);
        virtual ~wxLogOutput(){}
        void Flush(pml::log::Level level, const std::string&  sLog, const std::string& sPrefix) override;

    private:
        wxEvtHandler* m_pHandler;
};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_LOG, wxCommandEvent);



