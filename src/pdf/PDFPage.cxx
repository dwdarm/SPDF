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

std::string 
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
	std::string retVal;
	
	tod = new TextOutputDev (NULL, gTrue, 0.0, gFalse, gFalse);
	m_poppler_page->getDoc ()->displayPage(tod, getIndex ()+1,
                            xhdi, yhdi, 0, gFalse, gFalse, gFalse, NULL,
                            NULL, NULL, NULL, gFalse);
    text_page = tod->takeText();
    text = text_page->getText(xMin, yMin, xMax, yMax);
    
    if (text) {
		retVal = text->getCString ();
	}
	
    text_page->decRefCnt();
    delete (tod);
	
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
