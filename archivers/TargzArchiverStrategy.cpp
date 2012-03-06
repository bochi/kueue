/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "TargzArchiverStrategy.h"
#include "Utility.h"

using Utility::which;

TargzArchiverStrategy::TargzArchiverStrategy()
    : ArchiverStrategy("tar.gz", FileSignature())
{
}

TargzArchiverStrategy::~TargzArchiverStrategy()
{
}

void TargzArchiverStrategy::configure()
{
    addExtension(".tar.gz");
    addExtension(".tgz");
    addExtension(".cbg");
    setExecutables("tar");

    if (which("tar") != QString::null)
    {
        setExtractArguments("tar -xvzf @F");
        setListArguments("tar -tzf @F");
        setSupported();
    }
}
