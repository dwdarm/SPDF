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
#include <poppler/Outline.h>
#include <poppler/Link.h>
#include <poppler/goo/GooList.h>
#include <poppler/SplashOutputDev.h>

#include <iostream>
#include <stdio.h>

#include "PDFDocument.h"

static int
get_page_index_from_link_action (Catalog *catalog, LinkAction *action)
{
	int retVal = 1;
	
	// check link action type
			
	// if link action is go to destination
	if (action->getKind () == actionGoTo) {
		LinkGoTo *linkgoto = (LinkGoTo*) action;
		// if link to go is page number
		if (linkgoto->getDest ()) {
			if (linkgoto->getDest ()->isPageRef ()) {
				Ref ref = linkgoto->getDest ()->getPageRef();
				retVal = catalog->findPage (ref.num, ref.gen);
			} else {
				retVal = linkgoto->getDest ()->getPageNum ();
			}
		// if link to go is named
		} else {
			LinkDest *linkdest = catalog->findDest (linkgoto->getNamedDest());
			if (linkdest) {
				if (linkdest->isPageRef ()) {
					Ref ref = linkdest->getPageRef();
					retVal = catalog->findPage (ref.num, ref.gen);
				} else {
					retVal = linkdest->getPageNum ();
				}
			}
		}
			
	// if link action is named action
	} else if (action->getKind () == actionNamed) {
		LinkNamed *linknamed = (LinkNamed*) action;
		LinkDest *linkdest = catalog->findDest (linknamed->getName());
		if (linkdest) {
			if (linkdest->isPageRef ()) {
				Ref ref = linkdest->getPageRef();
				retVal = catalog->findPage (ref.num, ref.gen);
			} else {
				retVal = linkdest->getPageNum ();
			}
		}
				
	// if none of these	
	} else {
		retVal = 1;
	}
	
	return retVal;
}

std::vector<spdf::DocumentOutlineItem> 
outline_get_child (Catalog *catalog, OutlineItem *pitem)
{
	LinkAction *action = NULL;
	GooList *list = NULL;
	OutlineItem *item = NULL;
	spdf::UString title;
	int index = 1;
	std::vector<spdf::DocumentOutlineItem> retVal;
	std::vector<spdf::DocumentOutlineItem> child;
	
	pitem->open ();
	if (pitem->hasKids ()) {
		list = pitem->getKids ();
		if (list) {
			for (int i = 0; i < list->getLength (); i++) {
				item = (OutlineItem*) list->get (i);
				title = spdf::UString (item->getTitle (), item->getTitleLength());
				action = item->getAction ();
				if (action) {
					index = get_page_index_from_link_action (catalog, action);
				} else {
					index = 1;
				}
				child = outline_get_child (catalog, item);
				retVal.push_back (spdf::DocumentOutlineItem (title, index, child));
			}
		}
	}
	return retVal;
}

spdf::PDFDocument *
spdf::PDFDocument::openDocument (const std::string &filename, 
			const std::string &user_pass, const std::string &owner_pass) 
{
	spdf::PDFDocument *retVal = NULL;
	PDFDoc *poppler_document = NULL;
	
	if (!globalParams) {
		globalParams = new GlobalParams ();
	}
	
	poppler_document = new PDFDoc (new GooString (filename.data ()), 
								   new GooString (owner_pass.data ()), 
								   new GooString (user_pass.data ()), 
								   NULL);
	   
	if (poppler_document->isOk ()) {
		retVal = new spdf::PDFDocument ();
		retVal->m_poppler_document = poppler_document;
		retVal->m_is_loaded = true;
		retVal->m_type = PDF;
		retVal->m_pages = poppler_document->getNumPages();
	}
	
	return retVal;
}

spdf::PDFDocument::~PDFDocument ()
{
	delete (m_poppler_document);
}

spdf::DocumentPage *
spdf::PDFDocument::createPage (int index)
{
	spdf::PDFPage *retVal = NULL;
	Page *poppler_page = NULL;
	
	poppler_page = m_poppler_document->getCatalog ()->getPage(index+1);
	if (poppler_page) {
		retVal = new spdf::PDFPage ();
		retVal->m_poppler_page = poppler_page;
		retVal->m_index = index;
		retVal->m_width = poppler_page->getMediaWidth ();
		retVal->m_height = poppler_page->getMediaHeight ();
	}
	
	return retVal;
}

std::string 
spdf::PDFDocument::getTitle () const
{
	std::string retVal;
	GooString *title = NULL;
	char *ctitle = NULL;
	
	title = m_poppler_document->getFileName ();
	ctitle = title->getCString ();
	retVal = std::string (ctitle).substr
						   (std::string (ctitle).find_last_of("/\\")+1);
	
	return retVal;
}

spdf::DocumentOutline*
spdf::PDFDocument::getOutline () const 
{
	DocumentOutline *retVal = NULL;
	Catalog *catalog = NULL;
	Outline *outline = NULL;
	OutlineItem *item = NULL;
	GooList *list = NULL;
	LinkAction *action = NULL;
	UString title;
	int index = 1;
	std::vector<DocumentOutlineItem> child;
	
	catalog = m_poppler_document->getCatalog ();
	outline = m_poppler_document->getOutline ();
	if (outline) {
		list = outline->getItems ();
		if (list) {
			retVal = new DocumentOutline ();
			for (int i = 0; i < list->getLength (); i++) {
				item = (OutlineItem*) list->get (i);
				title = UString (item->getTitle (), item->getTitleLength());
				action = item->getAction ();
				if (action) {
					index = get_page_index_from_link_action (catalog, action);
				} else {
					index = 1;
				}
				child = outline_get_child (catalog, item);
				retVal->getRoot ().push_back (DocumentOutlineItem (title, index, child));
			}
		}
	}
	return retVal;
}
