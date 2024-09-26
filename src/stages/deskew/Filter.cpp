/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2015  Joseph Artsimovich <joseph.artsimovich@gmail.com>

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

#include <assert.h>
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QDomDocument>
#include <QDomElement>
#include "Filter.h"
#include "FilterUiInterface.h"
#include "OptionsWidget.h"
#include "Task.h"
#include "PageId.h"
#include "Settings.h"
#include "Params.h"
#include "ProjectReader.h"
#include "ProjectWriter.h"
#include "CacheDrivenTask.h"
#include "RelinkablePath.h"
#include "AbstractRelinker.h"
#include "orders/OrderByAngleProvider.h"
#include "orders/OrderByAngleObliqueProvider.h"
#include "orders/OrderByAngleHorProvider.h"
#include "orders/OrderByAngleVertProvider.h"
#include "CommandLine.h"

namespace deskew
{

Filter::Filter(PageSelectionAccessor const& page_selection_accessor)
    : m_ptrSettings(new Settings),
      m_selectedPageOrder(0)
{
    if (CommandLine::get().isGui())
    {
        m_ptrOptionsWidget.reset(new OptionsWidget(m_ptrSettings, page_selection_accessor));
    }

    typedef PageOrderOption::ProviderPtr ProviderPtr;

    ProviderPtr const default_order;
    ProviderPtr const order_by_angle(new OrderByAngleProvider(m_ptrSettings));
    ProviderPtr const order_by_angle_oblique(new OrderByAngleObliqueProvider(m_ptrSettings));
    ProviderPtr const order_by_angle_hor(new OrderByAngleHorProvider(m_ptrSettings));
    ProviderPtr const order_by_angle_vert(new OrderByAngleVertProvider(m_ptrSettings));
    m_pageOrderOptions.push_back(PageOrderOption(tr("Natural order"), default_order));
    m_pageOrderOptions.push_back(PageOrderOption(tr("Order by increasing angle"), order_by_angle));
    m_pageOrderOptions.push_back(PageOrderOption(tr("Order by oblique"), order_by_angle_oblique));
    m_pageOrderOptions.push_back(PageOrderOption(tr("Order by extension horizontally"), order_by_angle_hor));
    m_pageOrderOptions.push_back(PageOrderOption(tr("Order by extension vertically"), order_by_angle_vert));
}

Filter::~Filter()
{
}

QString
Filter::getName() const
{
    return QCoreApplication::translate("deskew::Filter", "Geometric Distortions");
}

PageView
Filter::getView() const
{
    return PAGE_VIEW;
}

int
Filter::selectedPageOrder() const
{
    return m_selectedPageOrder;
}

void
Filter::selectPageOrder(int option)
{
    assert((unsigned)option < m_pageOrderOptions.size());
    m_selectedPageOrder = option;
}

std::vector<PageOrderOption>
Filter::pageOrderOptions() const
{
    return m_pageOrderOptions;
}

void
Filter::performRelinking(AbstractRelinker const& relinker)
{
    m_ptrSettings->performRelinking(relinker);
}

void
Filter::preUpdateUI(FilterUiInterface* const ui, PageId const& page_id)
{
    m_ptrOptionsWidget->preUpdateUI(page_id, m_ptrSettings->getDistortionType(page_id));
    ui->setOptionsWidget(m_ptrOptionsWidget.get(), ui->KEEP_OWNERSHIP);
}

QDomElement
Filter::saveSettings(ProjectWriter const& writer, QDomDocument& doc) const
{
    QDomElement filter_el(doc.createElement("deskew"));

    writer.enumPages([this, &doc, &filter_el](PageId const& page_id, int numeric_id)
    {
        writePageSettings(doc, filter_el, page_id, numeric_id);
    });

    return filter_el;
}

void
Filter::loadSettings(ProjectReader const& reader, QDomElement const& filters_el)
{
    m_ptrSettings->clear();

    QDomElement const filter_el(filters_el.namedItem("deskew").toElement());

    QString const page_tag_name("page");
    QDomNode node(filter_el.firstChild());
    for (; !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
        {
            continue;
        }
        if (node.nodeName() != page_tag_name)
        {
            continue;
        }
        QDomElement const el(node.toElement());

        bool ok = true;
        int const id = el.attribute("id").toInt(&ok);
        if (!ok)
        {
            continue;
        }

        PageId const page_id(reader.pageId(id));
        if (page_id.isNull())
        {
            continue;
        }

        QDomElement const params_el(el.namedItem("params").toElement());
        if (params_el.isNull())
        {
            continue;
        }

        Params const params(params_el);
        m_ptrSettings->setPageParams(page_id, params);
    }
}

void
Filter::writePageSettings(
    QDomDocument& doc, QDomElement& filter_el,
    PageId const& page_id, int const numeric_id) const
{
    std::unique_ptr<Params> const params(m_ptrSettings->getPageParams(page_id));
    if (!params.get())
    {
        return;
    }

    QDomElement page_el(doc.createElement("page"));
    page_el.setAttribute("id", numeric_id);
    page_el.appendChild(params->toXml(doc, "params"));

    filter_el.appendChild(page_el);
}

IntrusivePtr<Task>
Filter::createTask(
    PageId const& page_id,
    IntrusivePtr<select_content::Task> const& next_task,
    bool const batch_processing, bool const debug)
{
    return IntrusivePtr<Task>(
               new Task(
                   IntrusivePtr<Filter>(this), m_ptrSettings,
                   next_task, page_id, batch_processing, debug
               )
           );
}

IntrusivePtr<CacheDrivenTask>
Filter::createCacheDrivenTask(
    IntrusivePtr<select_content::CacheDrivenTask> const& next_task)
{
    return IntrusivePtr<CacheDrivenTask>(
               new CacheDrivenTask(m_ptrSettings, next_task)
           );
}

} // namespace deskew
