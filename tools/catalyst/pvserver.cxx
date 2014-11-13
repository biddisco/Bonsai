/*=========================================================================

Program:   ParaView
Module:    pvserver.cxx

Copyright (c) Kitware, Inc.
All rights reserved.
See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "pvserver_common.h"
#ifndef BUILD_SHARED_LIBS
#include "pvStaticPluginsInit.h"
#endif

//----------------------------------------------------------------------------
extern "C" int bonsai_main(int argc, char* argv[], int commWorld)
{
#ifndef BUILD_SHARED_LIBS
  paraview_static_plugins_init();
#endif
  // Init current process type
  return RealMain(argc, argv, vtkProcessModule::PROCESS_SERVER, static_cast<int>(commWorld))? 0 : 1;
}
