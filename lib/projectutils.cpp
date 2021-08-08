/*
    Copyright 2021, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

#include "projectutils.h"

#include "project.h"

void ProjectUtils::addGuidesForSpacing(const Project *project, QVector<Guide> &guides, int horizontalSpacing, int verticalSpacing)
{
    for (int y = verticalSpacing; y < project->heightInPixels(); y += verticalSpacing) {
        for (int x = horizontalSpacing; x < project->widthInPixels(); x += horizontalSpacing) {
            const Guide guide(x, Qt::Vertical);
            if (!project->guides().contains(guide))
                guides.append(guide);
        }
        const Guide guide(y, Qt::Horizontal);
        if (!project->guides().contains(guide))
            guides.append(guide);
    }
}

QVector<Guide> ProjectUtils::uniqueGuides(const Project *project, const QVector<Guide> &guidesToAdd)
{
    QVector<Guide> addedGuides;
    for (const auto guide : guidesToAdd) {
        if (!project->guides().contains(guide))
            addedGuides.append(guide);
    }
    return addedGuides;
}
