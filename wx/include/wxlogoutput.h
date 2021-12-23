#pragma once
#include "log.h"
#include <string>
#include <fstream>
#include <wx/event.h>

class wxLogOutput : public pml::LogOutput
{
    public:
        explicit wxLogOutput(wxEvtHandler* pHandler);
        virtual ~wxLogOutput(){}
        void Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream) override;

    private:
        wxEvtHandler* m_pHandler;
};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_LOG, wxCommandEvent);



