/*
 * Copyright (c) 2010 Novell, Inc.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; version 3 of the license.
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail,
 * you may find current contact information at www.novell.com
 *
 * Author: Matt Barringer <mbarringer@suse.de>
 * Upstream: http://qsusestudio.sourceforge.net
 *
 */

#ifndef QSUSESTUDIO_GLOBAL_HPP
#define QSUSESTUDIO_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(QSUSESTUDIO_LIBRARY)
#  define QSUSESTUDIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QSUSESTUDIOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
