/*
 * Copyright (C) 2017, Fajar Dwi Darmanto <fajardwidarm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <poppler/PDFDoc.h>
#include <poppler/Page.h>
#include <poppler/SplashOutputDev.h>
#include <poppler/splash/SplashBitmap.h>
#include <poppler/TextOutputDev.h>
#include <poppler/goo/GooList.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

#include "PDFPage.h"

spdf::PDFPage::PDFPage () : spdf::DocumentPage ()
{
	m_poppler_page = NULL;
}

spdf::PDFPage::~PDFPage ()
{
	//delete (m_poppler_page);
}

spdf::Image *
spdf::PDFPage::render (double scale)
{
	spdf::Image *retVal = NULL;
	SplashBitmap *bitmap = NULL;
	SplashOutputDev *splashOutputDev = NULL;
	SplashColor background;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	
	//white background
    background[0] = 0xff;
    background[1] = 0xff;
	background[2] = 0xff;
	splashOutputDev = new SplashOutputDev (splashModeXBGR8, 4, gFalse, 
									background);
									
	splashOutputDev->setVectorAntialias (gTrue);
	
    splashOutputDev->setFreeTypeHinting (gTrue, gTrue);
    
	splashOutputDev->startDoc (m_poppler_page->getDoc ());
	
	m_poppler_page->getDoc ()->displayPage(splashOutputDev, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
                             
    bitmap = splashOutputDev->getBitmap ();
    
    retVal = new spdf::Image (bitmap->getWidth(), bitmap->getHeight(), 4);
    retVal->m_scale = scale;
    std::memcpy (retVal->m_data, bitmap->getDataPtr(),
							bitmap->getWidth() * bitmap->getHeight() * 4);                        
    
    delete (splashOutputDev);
            
	return retVal;
}

spdf::UString
spdf::PDFPage::searchText (Rect &rect, double scale)
{
	TextOutputDev *tod = NULL;
	TextPage *text_page = NULL;
	GooString *text = NULL;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	double xMin = rect.x; 
	double yMin = rect.y; 
	double xMax = rect.width;
	double yMax = rect.height;
	spdf::UString retVal;
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    text = text_page->getText(xMin, yMin, xMax, yMax);
    
    if (text) {
		retVal = spdf::UString (text->getCString (), text->getLength ());
	}
	
    text_page->decRefCnt();
    delete (tod);
	
	return retVal;
}

spdf::Rect 
spdf::PDFPage::searchRect (Rect &rect, std::string &text, double scale, PageSearchDirection dir)
{
	TextOutputDev *tod = NULL;
	TextPage *text_page = NULL;
	Unicode *utext = NULL;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	double xMin = rect.x; 
	double yMin = rect.y; 
	double xMax = rect.width + rect.x;
	double yMax = rect.height + rect.y;
	Rect retVal = {0, 0, 0, 0, 0};
	bool backward;
	bool result;
	
	utext = new Unicode[text.length ()];
	for (int i = 0; i < text.length (); i++) {
		utext[i] = text[i];
	}
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    
    if (dir == SEARCH_DIRECTION_NEXT) {
		backward = false;
	} else {
		backward = true;
	}
	
	result = text_page->findText (utext, text.length (), gFalse, gTrue, gFalse, gFalse, 
				   gTrue, backward, gFalse, &xMin, &yMin, &xMax, &yMax);
				   
	if (result) {
		retVal.index = getIndex ();
		retVal.x = (int) xMin;
		retVal.y = (int) yMin;
		retVal.width = (int) xMax - retVal.x;
		retVal.height = (int) yMax - retVal.y;
	}
    
    text_page->decRefCnt();
    delete (tod);
    delete [] (utext);
	
	return retVal;
}

std::vector<spdf::Rect> 
spdf::PDFPage::searchRect (std::string &text, double scale)
{
	std::vector<spdf::Rect> retVal;
	TextOutputDev *tod = NULL;
	TextPage *text_page = NULL;
	Unicode *utext = NULL;
	Rect rect;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	double xMin = 0.0; 
	double yMin = 0.0; 
	double xMax = 0.0;
	double yMax = 0.0;
	
	utext = new Unicode[text.length ()];
	for (int i = 0; i < text.length (); i++) {
		utext[i] = text[i];
	}
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    
    while (text_page->findText(utext, text.length (), gFalse, gTrue, gFalse, gFalse, 
					 gTrue, gFalse, gFalse, &xMin, &yMin, &xMax, &yMax))
	{
		rect.index = getIndex ();
		rect.x = (int) xMin;
		rect.y = (int) yMin;
		rect.width = (int) xMax - rect.x;
		rect.height = (int) yMax - rect.y;
		retVal.push_back (rect);
	}
    
    text_page->decRefCnt();
    delete (tod);
    delete [] (utext);
	
	return retVal;
}

std::vector<spdf::Rect> 
spdf::PDFPage::getSelectionRegion (PageSelectionStyle style, Rect &rect, double scale)
{
	TextOutputDev *tod = NULL;
	TextPage *text_page = NULL;
	PDFRectangle prect;
	SelectionStyle pstyle;
	GooList *list = NULL;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	std::vector<spdf::Rect> retVal;
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    
    prect.x1 = rect.x;
    prect.y1 = rect.y;
    prect.x2 = rect.width;
    prect.y2 = rect.height;
    
    if (style == SELECTION_STYLE_GLYPH) {
		pstyle = selectionStyleGlyph;
    } else if (style == SELECTION_STYLE_WORD) {
		pstyle = selectionStyleWord;
	} else {
		pstyle = selectionStyleLine;
	}
	
    list = text_page->getSelectionRegion(&prect, pstyle, 1.0);
    if (!list) {
		return retVal;
	}
	
	PDFRectangle *lrect = NULL;
	Rect rrect;
    for (int i = 0; i < list->getLength (); i++) {
		lrect = (PDFRectangle*) list->get (i);
		rrect.index = getIndex ();
		rrect.x = lrect->x1;
		rrect.y = lrect->y1;
		rrect.width = lrect->x2;
		rrect.height = lrect->y2;
		retVal.push_back (rrect);
	}
    
    text_page->decRefCnt();
    delete (tod);
    delete (list);
    
    return retVal;
}

std::string 
spdf::PDFPage::getSelectionText (PageSelectionStyle style, Rect &rect, double scale)
{
	TextOutputDev *tod = NULL;
	TextPage *text_page = NULL;
	PDFRectangle prect;
	SelectionStyle pstyle;
	double xhdi = 72.0 * scale;
	double yhdi = 72.0 * scale;
	GooString *text = NULL;
	std::string retVal;
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    
    prect.x1 = rect.x;
    prect.y1 = rect.y;
    prect.x2 = rect.width;
    prect.y2 = rect.height;
    
    if (style == SELECTION_STYLE_GLYPH) {
		pstyle = selectionStyleGlyph;
    } else if (style == SELECTION_STYLE_WORD) {
		pstyle = selectionStyleWord;
	} else {
		pstyle = selectionStyleLine;
	}
    
    text = text_page->getSelectionText(&prect, pstyle);
    if (text) {
		retVal = text->getCString ();
	}
    
    text_page->decRefCnt();
    delete (tod);
    
    return retVal;
}
