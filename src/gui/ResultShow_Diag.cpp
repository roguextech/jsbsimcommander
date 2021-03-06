/////////////////////////////////////////////////////////////////////////////
// Name:        ResultShow_Diag.h
// Purpose:     Shower for the result file
// Author:      Matthew Gong
// Created:     07/10/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// -*- C++ -*- generated by wxGlade 0.4cvs on Sun Jul 10 15:45:07 2005


#include <wx/wfstream.h>
#include <wx/mstream.h>
#include <wx/sstream.h>
#include <wx/txtstrm.h>

#include "ResultShow_Diag.h"

wxArrayString readStrsfromString(const wxString & str)
{
  wxStringInputStream mis(str);
  wxTextInputStream tis(mis);
  tis.SetStringSeparators(wxT("\t,"));

  wxArrayString list;
  
  
  while (!mis.Eof())
  {
    wxString tmp = tis.ReadWord();
    int i = 0;
    while (tmp[i] == ' ' || tmp[i] == '\"') ++i;
    int j = tmp.Length() - 1;
    while (tmp[j] == ' ' || tmp[j] == '\"') --j;
    tmp = tmp.Mid(i, j-i+1);
    list.Add(tmp);
  }

  return list;
}

void readNumsfromString(std::vector<double> &list, const wxString & str)
{
  wxStringInputStream mis(str);
  wxTextInputStream tis(mis);
  tis.SetStringSeparators(wxT("\t, "));

  list.clear();

  while (!mis.Eof())
  {
    wxString tmp = tis.ReadWord();
    double tmpd;
    if (tmp.ToDouble(&tmpd))
    {
      list.push_back(tmpd);
    }
    else
    {
      list.push_back(0);
    }
  }
}


class MyPlotCurveResult: public wxPlotCurve
{
public:
    MyPlotCurveResult(std::vector<double> &x, std::vector<double> &y, double width);

    virtual wxInt32 GetStartX()
        { return wxInt32(0); }
    virtual wxInt32 GetEndX()
        { return wxInt32((xvalue[size-1]-xvalue[0])/UPV); }
    
    virtual double GetY( wxInt32 x );

    double GetUPV() const
    {return UPV;}

    long GetIndex() const {return index;}
    void SetIndex(const long &l) {index = l;}

protected:
    std::vector<double> &xvalue;
    std::vector<double> &yvalue;
    int size;
    double UPV;
    int point;
    long index;
};

MyPlotCurveResult::MyPlotCurveResult(std::vector<double> &x, std::vector<double> &y, double width)
     : wxPlotCurve( 0, -5.0, 5.0 ), xvalue(x), yvalue(y), UPV(0.01), point(0) 
{
  size = xvalue.size()<=yvalue.size()?xvalue.size():yvalue.size();
  double h, l;
  h = l = yvalue[0];
  for (int i=1; i<size; ++i)
  {
    double tmp = yvalue[i];
    if (tmp > h)
    {
      h = tmp;
    }
    else if (tmp < l)
    {
      l = tmp;
    }
  }

  double tmp = (h - l)*0.05;
  if (tmp == 0)
    tmp = 0.0001;
  UPV = (x[size-1] - x[0]) / width;
  SetStartY(l - tmp);
  SetEndY(h + tmp);

  SetStartXVal(x[0]);
}


double MyPlotCurveResult::GetY( wxInt32 x )
{
  double dx = x * UPV + xvalue[0];

  while (point > 0 && xvalue[point]>=dx )
    --point;
  while (point+1 < size-1 && xvalue[point+1]<dx )
    ++point;

  double scale = (dx-xvalue[point]) / (xvalue[point+1]-xvalue[point]);
  if (scale < 0) scale = 0;
  if (scale > 1) scale = 1;

  return (yvalue[point+1]-yvalue[point])*scale + yvalue[point];
}

ResultShowDialog::ResultShowDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    // begin wxGlade: ResultShowDialog::ResultShowDialog
    notebook_main = new wxNotebook(this, -1);
    notebook_main_pane_combo = new wxPanel(notebook_main, -1);
    notebook_main_pane_column = new wxPanel(notebook_main, -1);
    list_box_prop = new wxListCtrl(notebook_main_pane_combo, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxSUNKEN_BORDER);
    check_box_samerange = new wxCheckBox(notebook_main_pane_combo, ID_SAMERANGE, _("Same Range"));
    btn_update = new wxButton(notebook_main_pane_combo, ID_UPDATE, _("Update"));
    curve_rslt = new wxPlotWindow(notebook_main_pane_combo, ID_CURVE, wxDefaultPosition, wxSize(600,400), wxPLOT_Y_AXIS|wxPLOT_X_AXIS);
    list_box_prop_single = new wxListCtrl(notebook_main_pane_column, ID_LISTBOX_SINGLE, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxSUNKEN_BORDER|wxLC_SINGLE_SEL);
    curve_rslts[0] = new wxPlotWindow(notebook_main_pane_column, ID_CURVE0, wxDefaultPosition, wxDefaultSize, wxPLOT_Y_AXIS);
    curve_rslts[1] = new wxPlotWindow(notebook_main_pane_column, ID_CURVE1, wxDefaultPosition, wxDefaultSize, wxPLOT_Y_AXIS);
    curve_rslts[2] = new wxPlotWindow(notebook_main_pane_column, ID_CURVE2, wxDefaultPosition, wxDefaultSize, wxPLOT_Y_AXIS);
    curve_rslts[3] = new wxPlotWindow(notebook_main_pane_column, ID_CURVE3, wxDefaultPosition, wxDefaultSize, wxPLOT_Y_AXIS|wxPLOT_X_AXIS);
    const wxString radio_box_choices[] = {
        _("curve 1"),
        _("curve 2"),
        _("curve 3"),
        _("curve 4")
    };
    radio_select = new wxRadioBox(notebook_main_pane_column, ID_RADIO_SELECT, _("Select"), wxDefaultPosition, wxDefaultSize, 4, radio_box_choices, 1);
    btn_update2 = new wxButton(notebook_main_pane_column, ID_UPDATE2, _("Update"));
    button_ok = new wxButton(this, wxID_OK, _("OK"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(ResultShowDialog, wxDialog)
    // begin wxGlade: ResultShowDialog::event_table
    EVT_LIST_ITEM_SELECTED(ID_LISTBOX, ResultShowDialog::OnSelectListBox)
    EVT_LIST_ITEM_DESELECTED(ID_LISTBOX, ResultShowDialog::OnDeselectListBox)
    EVT_CHECKBOX(ID_SAMERANGE, ResultShowDialog::OnCheckSameRange) 
    EVT_LIST_ITEM_SELECTED(ID_LISTBOX_SINGLE, ResultShowDialog::OnSelectListBoxSingle)
    // end wxGlade
    EVT_PLOT_AREA_CLICKED  (ID_CURVE0, ResultShowDialog::OnSelectPlotWindow0)
    EVT_PLOT_AREA_CLICKED  (ID_CURVE1, ResultShowDialog::OnSelectPlotWindow1)
    EVT_PLOT_AREA_CLICKED  (ID_CURVE2, ResultShowDialog::OnSelectPlotWindow2)
    EVT_PLOT_AREA_CLICKED  (ID_CURVE3, ResultShowDialog::OnSelectPlotWindow3)
    EVT_RADIOBOX(ID_RADIO_SELECT, ResultShowDialog::OnRadioSelect)
    EVT_BUTTON(ID_UPDATE, ResultShowDialog::OnUpdate) 
    EVT_BUTTON(ID_UPDATE2, ResultShowDialog::OnUpdate2) 
    EVT_PLOT_MOTION(  wxID_ANY,     ResultShowDialog::OnUpdate4Curves)
END_EVENT_TABLE();


void ResultShowDialog::OnSelectListBox(wxListEvent &event)
{
    long index = event.GetIndex();
    int w, h;
    curve_rslt->GetClientSize(&w, &h);
    MyPlotCurveResult * newcurve = new MyPlotCurveResult(lines[0], lines[index], w - 100 - 5);
    newcurve->SetIndex(index);
    newcurve->SetXLabel(wxT("Time"));
    newcurve->SetYLabel(list_box_prop->GetItemText(index));
    curve_rslt->SetUnitsPerValue( newcurve->GetUPV() );
    curve_rslt->Add(newcurve);
    curve.push_back(newcurve);
    curve_rslt->Layout();
    curve_rslt->RedrawEverything();
}

void ResultShowDialog::OnDeselectListBox(wxListEvent &event)
{
  long index = event.GetIndex();
  wxString str = list_box_prop->GetItemText(index);
  for (bool flag = true; flag; )
  {
    flag = false;
    for (std::vector< MyPlotCurveResult * >::iterator i = curve.begin(); i != curve.end(); ++i )
    {
      if ((*i)->GetIndex() == index)
      {
        curve_rslt->Delete(*i);
        curve.erase(i);
	flag = true;
        break;
      }
    }
  }
}

// wxGlade: add ResultShowDialog event handlers

void ResultShowDialog::OnCheckSameRange(wxCommandEvent &event)
{
  if (check_box_samerange->GetValue())
  {
    curve_rslt->SameRange(true);
  }
  else
  {
    curve_rslt->SameRange(false);
  }
  curve_rslt->RedrawEverything();
}

void ResultShowDialog::OnSelectListBoxSingle(wxListEvent &event)
{
    int i = event.GetIndex();
    int j = radio_select->GetSelection();
    if (curve_symbols[j] == i)
      return;
    if (curves[j])
    {
      curve_rslts[j]->Delete(curves[j]);
    }
    int w, h;
    curve_rslts[j]->GetClientSize(&w, &h);
    curve_symbols[j] = i;
    curves[j] = new MyPlotCurveResult(lines[0], lines[i], w - 100 - 5);
    curves[j]->SetXLabel(wxT("Time"));
    curves[j]->SetYLabel(list_box_prop->GetItemText(i));
    curve_rslts[j]->SetUnitsPerValue( curves[j]->GetUPV() );
    curve_rslts[j]->Add(curves[j]);
    curve_rslts[j]->Layout();
    curve_rslts[j]->RedrawEverything();
}

void ResultShowDialog::OnSelectPlotWindow0(wxPlotEvent &event)
{
  radio_select->SetSelection(0);
  list_box_prop_single->SetItemState(curve_symbols[0], wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  list_box_prop_single->EnsureVisible(curve_symbols[0]);
}

void ResultShowDialog::OnSelectPlotWindow1(wxPlotEvent &event)
{
  radio_select->SetSelection(1);
  list_box_prop_single->SetItemState(curve_symbols[1], wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  list_box_prop_single->EnsureVisible(curve_symbols[1]);
}

void ResultShowDialog::OnSelectPlotWindow2(wxPlotEvent &event)
{
  radio_select->SetSelection(2);
  list_box_prop_single->SetItemState(curve_symbols[2], wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  list_box_prop_single->EnsureVisible(curve_symbols[2]);
}

void ResultShowDialog::OnSelectPlotWindow3(wxPlotEvent &event)
{
  radio_select->SetSelection(3);
  list_box_prop_single->SetItemState(curve_symbols[3], wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  list_box_prop_single->EnsureVisible(curve_symbols[3]);
}

void ResultShowDialog::OnRadioSelect(wxCommandEvent &event)
{
  int j = radio_select->GetSelection();
  list_box_prop_single->SetItemState(curve_symbols[j], wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  list_box_prop_single->EnsureVisible(curve_symbols[j]);
}

void ResultShowDialog::OnUpdate(wxCommandEvent &event)
{
  curve_rslt->Refresh();
  curve_rslt->Update();
}

void ResultShowDialog::OnUpdate2(wxCommandEvent &event)
{
  for (int i = 0; i < 4; ++i)
  {
    curve_rslts[i]->Refresh();
    curve_rslts[i]->Update();
  }
}
void ResultShowDialog::OnUpdate4Curves(wxPlotEvent &event)
{
  int id = event.GetId();
  if (id == ID_CURVE)
    return;

  wxCoord x, y;
  x = event.GetX();
  y = event.GetY();
  if (id != ID_CURVE0)
    curve_rslts[0]->MakeMark(event.GetX());
  if (id != ID_CURVE1)
    curve_rslts[1]->MakeMark(event.GetX());
  if (id != ID_CURVE2)
    curve_rslts[2]->MakeMark(event.GetX());
  if (id != ID_CURVE3)
    curve_rslts[3]->MakeMark(event.GetX());
}

void ResultShowDialog::set_properties()
{
    // begin wxGlade: ResultShowDialog::set_properties
    SetMinSize(wxSize(800, 600));
    list_box_prop->SetMinSize(wxSize(150, -1));
    list_box_prop->SetMaxSize(wxSize(200, -1));
    wxListItem itemCol;
    itemCol.SetText(_T("Symbol"));
    itemCol.SetImage(-1);
    list_box_prop->InsertColumn(0, itemCol);
    check_box_samerange->SetValue(false);
    list_box_prop_single->SetMinSize(wxSize(150, -1));
    list_box_prop_single->SetMaxSize(wxSize(200, -1));
    list_box_prop_single->InsertColumn(0, itemCol);
    radio_select->SetSelection(3);
    // end wxGlade
    curves[0]=curves[1]=curves[2]=curves[3]= NULL;
    curve_symbols[0]=curve_symbols[1]=curve_symbols[2]=curve_symbols[3]= -1;
}


void ResultShowDialog::do_layout()
{
    // begin wxGlade: ResultShowDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
    sizer_2->Add(list_box_prop, 1, wxALL|wxEXPAND, 5);
    sizer_2->Add(check_box_samerange, 0, wxALL|wxEXPAND, 5);
    sizer_2->Add(btn_update, 0, wxALL|wxEXPAND, 5);
    sizer_3->Add(sizer_2, 0, wxEXPAND, 5);
    sizer_3->Add(curve_rslt, 1, wxALL|wxEXPAND, 5);
    notebook_main_pane_combo->SetAutoLayout(true);
    notebook_main_pane_combo->SetSizer(sizer_3);
    sizer_3->Fit(notebook_main_pane_combo);
    sizer_3->SetSizeHints(notebook_main_pane_combo);

    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxVERTICAL);
    sizer_6->Add(radio_select, 0, wxALL|wxEXPAND, 5);
    sizer_6->Add(list_box_prop_single, 1, wxALL|wxEXPAND, 5);
    sizer_6->Add(btn_update2, 0, wxALL|wxEXPAND, 5);
    sizer_5->Add(sizer_6, 0, wxEXPAND, 5);
    sizer_4->Add(curve_rslts[0], 3, wxEXPAND, 5);
    sizer_4->Add(curve_rslts[1], 3, wxEXPAND, 5);
    sizer_4->Add(curve_rslts[2], 3, wxEXPAND, 5);
    sizer_4->Add(curve_rslts[3], 4, wxEXPAND, 5);
    sizer_5->Add(sizer_4, 1, wxALL|wxEXPAND, 5);
    notebook_main_pane_column->SetAutoLayout(true);
    notebook_main_pane_column->SetSizer(sizer_5);
    sizer_5->Fit(notebook_main_pane_column);
    sizer_5->SetSizeHints(notebook_main_pane_column);

    notebook_main->AddPage(notebook_main_pane_combo, _("Combo"));
    notebook_main->AddPage(notebook_main_pane_column, _("Column"));
    sizer_1->Add(notebook_main, 1, wxALL|wxEXPAND, 5);
    sizer_1->Add(button_ok, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 10);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    curve_rslt->Layout();
    curve_rslts[0]->Layout();
    curve_rslts[1]->Layout();
    curve_rslts[2]->Layout();
    curve_rslts[3]->Layout();
    // end wxGlade
}

bool ResultShowDialog::Load (const wxString & fn)
{
  wxFileInputStream input( fn );
  if (!input.Ok())
    return false;
  wxTextInputStream text( input );

  names = readStrsfromString(text.ReadLine());
  list_box_prop->DeleteAllItems();
  list_box_prop_single->DeleteAllItems();
  wxListItem item;
  for (int i = 0; i < names.GetCount(); ++i)
  {
    item.SetId(i);
    item.SetColumn(0);
    item.SetText(names[i]);
    item.SetImage(-1);
    list_box_prop->InsertItem(item);
    list_box_prop_single->InsertItem(item);
  }
  list_box_prop->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  list_box_prop_single->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  
  std::vector<double> tmp;
  tmp.reserve(120*5);
  for (int i = 0; i < names.GetCount(); ++i)
    lines.push_back(tmp);
  
  std::vector<double> line;
  line.reserve(names.GetCount() + 2);

  while (!input.Eof())
  {
    readNumsfromString(line, text.ReadLine());
    if (line.size() == names.GetCount())
    {
      std::vector< std::vector<double> >::iterator i = lines.begin();
      std::vector<double>::iterator j = line.begin();
      for (; i != lines.end(); ++i, ++j)
      {
        (*i).push_back(*j);
      }
    }
  }

  return true;
}

