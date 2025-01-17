#include <wx/uri.h>
#include <wx/protocol/http.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>
#include <wx/notebook.h>

#include "PlayListItemCURL.h"
#include "PlayListItemCURLPanel.h"

#include <log4cpp/Category.hh>

PlayListItemCURL::PlayListItemCURL(wxXmlNode* node) : PlayListItem(node)
{
    _started = false;
    _url = "";
    _curltype = "GET";
    _body = "";
    PlayListItemCURL::Load(node);
}

void PlayListItemCURL::Load(wxXmlNode* node)
{
    PlayListItem::Load(node);
    _url = node->GetAttribute("URL", "");
    _curltype = node->GetAttribute("Type", "GET");
    _body = node->GetAttribute("Body", "");
}

PlayListItemCURL::PlayListItemCURL() : PlayListItem()
{
    _type = "PLICURL";
    _started = false;
    _url = "";
    _curltype = "GET";
    _body = "";
}

PlayListItem* PlayListItemCURL::Copy() const
{
    PlayListItemCURL* res = new PlayListItemCURL();
    res->_url = _url;
    res->_curltype = _curltype;
    res->_body = _body;
    res->_started = false;
    PlayListItem::Copy(res);

    return res;
}

wxXmlNode* PlayListItemCURL::Save()
{
    wxXmlNode * node = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, GetType());

    node->AddAttribute("URL", _url);
    node->AddAttribute("Type", _curltype);
    node->AddAttribute("Body", _body);

    PlayListItem::Save(node);

    return node;
}

std::string PlayListItemCURL::GetTitle() const
{
    return "CURL";
}

void PlayListItemCURL::Configure(wxNotebook* notebook)
{
    notebook->AddPage(new PlayListItemCURLPanel(notebook, this), GetTitle(), true);
}

std::string PlayListItemCURL::GetNameNoTime() const
{
    if (_name != "") return _name;

	wxURI uri(_url);
	if (uri.GetServer() != "")
	{
		return uri.GetServer().ToStdString();
	}
	
    return "CURL";
}

std::string PlayListItemCURL::GetTooltip()
{
    return GetTagHint();
}

void PlayListItemCURL::Frame(uint8_t* buffer, size_t size, size_t ms, size_t framems, bool outputframe)
{
    if (ms >= _delay && !_started)
    {
        _started = true;

        std::string url = ReplaceTags(_url);
        std::string body = ReplaceTags(_body);

        static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
        logger_base.info("Calling URL %s.", (const char *)url.c_str());

        if (url == "")
        {
            logger_base.warn("URL '' invalid.", (const char *)url.c_str());
            return;
        }

        wxURI uri(url);

        wxHTTP http;
        http.SetTimeout(10);
        http.SetMethod(_curltype);
        if (http.Connect(uri.GetServer()))
        {
            if (_curltype == "POST")
            {
                http.SetPostText("application/x-www-form-urlencoded", _body);
            }
            wxString page = uri.GetPath() + "?" + uri.GetQuery();
            wxInputStream *httpStream = http.GetInputStream(page);
            if (http.GetError() == wxPROTO_NOERR)
            {
                wxString res;
                wxStringOutputStream out_stream(&res);
                httpStream->Read(out_stream);

                logger_base.info("CURL: %s", (const char *)res.c_str());
            }
            else
            {
                logger_base.error("CURL: Error getting page %s from %s.", (const char*)page.c_str(), (const char *)uri.GetServer().c_str());
            }

            if (_curltype == "POST")
            {
                http.SetPostText("", "");
            }
            wxDELETE(httpStream);
	    http.Close();
        }
        else
        {
            logger_base.error("CURL: Error connecting to %s.", (const char *)uri.GetServer().c_str());
        }
    }
}

void PlayListItemCURL::Start(long stepLengthMS)
{
    PlayListItem::Start(stepLengthMS);

    _started = false;
}
