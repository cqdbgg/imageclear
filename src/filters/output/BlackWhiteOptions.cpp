/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BlackWhiteOptions.h"
#include <QDomDocument>
#include <QDomElement>
#include <QString>

namespace output
{

BlackWhiteOptions::BlackWhiteOptions()
    :   m_thresholdMethod(OTSU),
        m_dimmingColoredCoef(0.0),
        m_thresholdAdjustment(0),
        m_thresholdWindowSize(200),
        m_thresholdCoef(0.3),
        m_kmeansCount(0),
        m_kmeansSat(0.0),
        m_kmeansNorm(0.0),
        m_kmeansBG(0.0),
        m_coloredMaskCoef(0.0)
{
}

BlackWhiteOptions::BlackWhiteOptions(QDomElement const& el)
    :   m_thresholdMethod(parseThresholdMethod(el.attribute("thresholdMethod"))),
        m_dimmingColoredCoef(el.attribute("dimmingColoredCoef").toDouble()),
        m_thresholdAdjustment(el.attribute("thresholdAdj").toInt()),
        m_thresholdWindowSize(el.attribute("thresholdWinSize").toInt()),
        m_thresholdCoef(el.attribute("thresholdCoef").toDouble()),
        m_kmeansCount(el.attribute("kmeans").toInt()),
        m_kmeansSat(el.attribute("kmeansSat").toDouble()),
        m_kmeansNorm(el.attribute("kmeansNorm").toDouble()),
        m_kmeansBG(el.attribute("kmeansBG").toDouble()),
        m_coloredMaskCoef(el.attribute("coloredMaskCoef").toDouble())
{
    if (m_dimmingColoredCoef < -1.0 || m_dimmingColoredCoef > 2.0)
    {
        m_dimmingColoredCoef = 0.0;
    }
    if (m_thresholdWindowSize == 0)
    {
        m_thresholdWindowSize = 200;
    }
    if (m_thresholdCoef < 0.0)
    {
        m_thresholdCoef = 0.0;
    }
    if (m_kmeansCount < 0)
    {
        m_kmeansCount = 0;
    }
    if (m_kmeansSat < 0.0 || m_kmeansSat > 1.0)
    {
        m_kmeansSat = 0.0;
    }
    if (m_kmeansNorm < 0.0 || m_kmeansNorm > 1.0)
    {
        m_kmeansNorm = 0.0;
    }
    if (m_kmeansBG < 0.0 || m_kmeansBG > 1.0)
    {
        m_kmeansBG = 0.0;
    }
    if (m_coloredMaskCoef < 0.0 || m_coloredMaskCoef > 1.0)
    {
        m_coloredMaskCoef = 0.0;
    }
}

QDomElement
BlackWhiteOptions::toXml(QDomDocument& doc, QString const& name) const
{
    QDomElement el(doc.createElement(name));
    el.setAttribute("thresholdMethod", formatThresholdMethod(m_thresholdMethod));
    el.setAttribute("dimmingColoredCoef", m_dimmingColoredCoef);
    el.setAttribute("thresholdAdj", m_thresholdAdjustment);
    el.setAttribute("thresholdWinSize", m_thresholdWindowSize);
    el.setAttribute("thresholdCoef", m_thresholdCoef);
    el.setAttribute("kmeans", m_kmeansCount);
    el.setAttribute("kmeansSat", m_kmeansSat);
    el.setAttribute("kmeansNorm", m_kmeansNorm);
    el.setAttribute("kmeansBG", m_kmeansBG);
    el.setAttribute("coloredMaskCoef", m_coloredMaskCoef);
    return el;
}

bool
BlackWhiteOptions::operator==(BlackWhiteOptions const& other) const
{
    if (m_thresholdMethod != other.m_thresholdMethod)
    {
        return false;
    }
    if (m_dimmingColoredCoef != other.m_dimmingColoredCoef)
    {
        return false;
    }
    if (m_thresholdAdjustment != other.m_thresholdAdjustment)
    {
        return false;
    }
    if (m_thresholdWindowSize != other.m_thresholdWindowSize)
    {
        return false;
    }
    if (m_thresholdCoef != other.m_thresholdCoef)
    {
        return false;
    }
    if (m_kmeansCount != other.m_kmeansCount)
    {
        return false;
    }
    if (m_kmeansSat != other.m_kmeansSat)
    {
        return false;
    }
    if (m_kmeansNorm != other.m_kmeansNorm)
    {
        return false;
    }
    if (m_kmeansBG != other.m_kmeansBG)
    {
        return false;
    }
    if (m_coloredMaskCoef != other.m_coloredMaskCoef)
    {
        return false;
    }

    return true;
}

bool
BlackWhiteOptions::operator!=(BlackWhiteOptions const& other) const
{
    return !(*this == other);
}

ThresholdFilter
BlackWhiteOptions::parseThresholdMethod(QString const& str)
{
    if (str == "mean")
    {
        return MEANDELTA;
    }
    else if (str == "niblack")
    {
        return NIBLACK;
    }
    else if (str == "gatos")
    {
        return GATOS;
    }
    else if (str == "sauvola")
    {
        return SAUVOLA;
    }
    else if (str == "wolf")
    {
        return WOLF;
    }
    else if (str == "bradley")
    {
        return BRADLEY;
    }
    else if (str == "edgeplus")
    {
        return EDGEPLUS;
    }
    else if (str == "blurdiv")
    {
        return BLURDIV;
    }
    else if (str == "edgediv")
    {
        return EDGEDIV;
    }
    else if (str == "multiscale")
    {
        return MSCALE;
    }
    else
    {
        return OTSU;
    }
}

QString
BlackWhiteOptions::formatThresholdMethod(ThresholdFilter type)
{
    QString str = "";
    switch (type)
    {
    case OTSU:
        str = "otsu";
        break;
    case MEANDELTA:
        str = "mean";
        break;
    case NIBLACK:
        str = "niblack";
        break;
    case GATOS:
        str = "gatos";
        break;
    case SAUVOLA:
        str = "sauvola";
        break;
    case WOLF:
        str = "wolf";
        break;
    case BRADLEY:
        str = "bradley";
        break;
    case EDGEPLUS:
        str = "edgeplus";
        break;
    case BLURDIV:
        str = "blurdiv";
        break;
    case EDGEDIV:
        str = "edgediv";
        break;
    case MSCALE:
        str = "multiscale";
        break;
    }
    return str;
}

} // namespace output
