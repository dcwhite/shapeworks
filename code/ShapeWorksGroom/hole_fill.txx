/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: hole_fill.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_hole_fill_txx
#define __st_hole_fill_txx

#include "hole_fill.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace shapetools
{

template <class T, unsigned int D>
hole_fill<T,D>::hole_fill(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    //PARAMSET(pf, f, "foreground", 0, ok, 0.0);
    this->m_foreground = 0.0;
    elem = docHandle.FirstChild( "foreground" ).Element();
    if (elem)
    {
      this->m_foreground = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "hole_fill:: missing parameters" << std::endl;
    }

    //PARAMSET(pf, g, "background", 0, ok, 1.0);
    this->m_background = 1.0;
    elem = docHandle.FirstChild( "background" ).Element();
    if (elem)
    {
      this->m_background = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "hole_fill:: missing parameters" << std::endl;
    }

    // NOTE: this method always sets the background seed point to 0, 0, 0.
    this->m_seed.Fill(0);
  }
}


template <class T, unsigned int D> 
void hole_fill<T,D>::operator()(typename image_type::Pointer img)
{
  // Flood fill the background with the foreground value.
  typename itk::ConnectedThresholdImageFilter<image_type, image_type>::Pointer
    ccfilter = itk::ConnectedThresholdImageFilter<image_type, image_type>::New();
  ccfilter->SetInput(img);
  ccfilter->SetLower(m_background);
  ccfilter->SetUpper(m_background);
  ccfilter->SetSeed(m_seed);
  ccfilter->SetReplaceValue(m_foreground);
  ccfilter->Update();

  // "or" the original image with the inverse (foreground/background flipped)
  // of the flood-filled copy.

  itk::ImageRegionConstIterator<image_type> rit(ccfilter->GetOutput(),
                                                img->GetBufferedRegion());
  itk::ImageRegionIterator<image_type> it(img,img->GetBufferedRegion());
  for (; !rit.IsAtEnd(); ++rit, ++it)
    {
    if (rit.Get() == m_background)
      {
      it.Set(m_foreground);
      }
    }
}

} // end namespace

#endif
