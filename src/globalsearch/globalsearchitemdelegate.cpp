/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "globalsearchitemdelegate.h"
#include "globalsearchwidget.h"
#include "searchprovider.h"
#include "core/logging.h"

#include <QApplication>
#include <QPainter>


const int GlobalSearchItemDelegate::kHeight = SearchProvider::kArtHeight;
const int GlobalSearchItemDelegate::kMargin = 2;
const int GlobalSearchItemDelegate::kArtMargin = 6;

GlobalSearchItemDelegate::GlobalSearchItemDelegate(GlobalSearchWidget* widget)
  : QStyledItemDelegate(widget),
    widget_(widget)
{
  no_cover_ = QPixmap::fromImage(SearchProvider::ScaleAndPad(QImage(":nocover.png")));
}

QSize GlobalSearchItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setHeight(kHeight + kMargin*2);
  return size;
}

void GlobalSearchItemDelegate::paint(QPainter* p,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
  const SearchProvider::Result result =
      index.data(GlobalSearchWidget::Role_Result).value<SearchProvider::Result>();
  const Song& m = result.metadata_;
  const bool selected = option.state & QStyle::State_Selected;

  widget_->LazyLoadArt(index);

  QFont bold_font = option.font;
  bold_font.setBold(true);

  QColor pen = option.palette.color(selected ? QPalette::HighlightedText : QPalette::Text);
  QColor light_pen = pen;
  pen.setAlpha(200);
  light_pen.setAlpha(128);

  // Draw the background
  const QStyleOptionViewItemV3* vopt = qstyleoption_cast<const QStyleOptionViewItemV3*>(&option);
  const QWidget* widget = vopt->widget;
  QStyle* style = widget->style() ? widget->style() : QApplication::style();
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, p, widget);

  // Draw the album art.  This will already be the correct size.
  const QRect rect = option.rect;
  const QRect art_rect(rect.left() + kMargin, rect.top() + kMargin, kHeight, kHeight);

  QPixmap art = index.data(Qt::DecorationRole).value<QPixmap>();
  if (art.isNull())
    art = no_cover_;

  p->drawPixmap(art_rect, art);

  // Position text
  QRect text_rect(art_rect.right() + kArtMargin, art_rect.top(),
                  rect.right() - art_rect.right() - kArtMargin, kHeight);
  QRect text_rect_1(text_rect.adjusted(0, 0, 0, -kHeight/2));
  QRect text_rect_2(text_rect.adjusted(0, kHeight/2, 0, 0));

  QString line_1;
  QString line_2;

  // The text we draw depends on the type of result.
  switch (result.type_) {
  case SearchProvider::Result::Type_Track: {
    // Title
    line_1 += m.title() + " ";

    // Artist - Album - Track n
    if (!m.artist().isEmpty()) {
      line_2 += m.artist();
    } else if (!m.albumartist().isEmpty()) {
      line_2 += m.albumartist();
    }

    if (!m.album().isEmpty()) {
      line_2 += "  -  " + m.album();
    }

    if (m.track() > 0) {
      line_2 += "  -  " + tr("track %1").arg(m.track());
    }

    break;
  }

  case SearchProvider::Result::Type_Album: {
    // Line 1 is Artist - Album
    // Artist
    if (!m.albumartist().isEmpty())
      line_1 += m.albumartist();
    else if (m.is_compilation())
      line_1 += tr("Various Artists");
    else if (!m.artist().isEmpty())
      line_1 += m.artist();
    else
      line_1 += tr("Unknown");

    // Dash
    line_1 += "  -  ";

    // Album
    if (m.album().isEmpty())
      line_1 += tr("Unknown");
    else
      line_1 += m.album();

    // Line 2 is <n> tracks
    line_2 += QString::number(result.album_size_) + " ";
    line_2 += tr(result.album_size_ == 1 ? "track" : "tracks");
    break;
  }
  }

  p->setFont(bold_font);
  p->setPen(pen);
  p->drawText(text_rect_1, Qt::TextSingleLine | Qt::AlignVCenter, line_1);

  p->setFont(option.font);
  p->setPen(light_pen);
  p->drawText(text_rect_2, Qt::TextSingleLine | Qt::AlignVCenter, line_2);
}
