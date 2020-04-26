/**
 * Nested list helper
 *
 * Copyright 2008  Stephen Kelly <steveire@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "nestedlisthelper_p.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QTextList>
#include <QTextBlock>

#include "ktextedit.h"

NestedListHelper::NestedListHelper(QTextEdit *te)
{
    textEdit = te;
}

NestedListHelper::~NestedListHelper()
{
}

bool NestedListHelper::handleKeyPressEvent(QKeyEvent *event)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.currentList()) {
        return false;
    }

    if (event->key() == Qt::Key_Backspace && !cursor.hasSelection() && cursor.atBlockStart() && canDedent()) {
        handleOnIndentLess();
        return true;
    }

    if (event->key() == Qt::Key_Return && !cursor.hasSelection() && cursor.block().text().isEmpty() && canDedent()) {
        handleOnIndentLess();
        return true;
    }

    if (event->key() == Qt::Key_Tab && (cursor.atBlockStart() || cursor.hasSelection()) && canIndent()) {
        handleOnIndentMore();
        return true;
    }

    return false;
}

bool NestedListHelper::canIndent() const
{
    const QTextCursor cursor = topOfSelection();
    const QTextBlock block = cursor.block();
    if (!block.isValid()) {
        return false;
    }
    if (!block.textList()) {
        return true;
    }
    const QTextBlock prevBlock = block.previous();
    if (!prevBlock.textList()) {
        return false;
    }
    return block.textList()->format().indent() <= prevBlock.textList()->format().indent();
}

bool NestedListHelper::canDedent() const
{
    const QTextCursor cursor = bottomOfSelection();
    const QTextBlock block = cursor.block();
    if (!block.isValid()) {
        return false;
    }
    if (!block.textList() || block.textList()->format().indent() <= 0) {
        return false;
    }
    const QTextBlock nextBlock = block.next();
    if (!nextBlock.textList()) {
        return true;
    }
    return block.textList()->format().indent() >= nextBlock.textList()->format().indent();
}

bool NestedListHelper::handleAfterDropEvent(QDropEvent *dropEvent)
{
    Q_UNUSED(dropEvent);
    QTextCursor cursor = topOfSelection();

    QTextBlock droppedBlock = cursor.block();
    int firstDroppedItemIndent = droppedBlock.textList()->format().indent();

    int minimumIndent = droppedBlock.previous().textList()->format().indent();

    if (firstDroppedItemIndent < minimumIndent) {
        cursor = QTextCursor(droppedBlock);
        QTextListFormat fmt = droppedBlock.textList()->format();
        fmt.setIndent(minimumIndent);
        QTextList *list = cursor.createList(fmt);

        int endOfDrop = bottomOfSelection().position();
        while (droppedBlock.next().position() < endOfDrop) {
            droppedBlock = droppedBlock.next();
            if (droppedBlock.textList()->format().indent() != firstDroppedItemIndent) {

                // new list?
            }
            list->add(droppedBlock);
        }
//         list.add( droppedBlock );
    }

    return true;
}

void NestedListHelper::processList(QTextList *list)
{
    QTextBlock block = list->item(0);
    int thisListIndent = list->format().indent();

    QTextCursor cursor = QTextCursor(block);
    list = cursor.createList(list->format());
    bool processingSubList  = false;
    while (block.next().textList() != nullptr) {
        block = block.next();

        QTextList *nextList = block.textList();
        int nextItemIndent = nextList->format().indent();
        if (nextItemIndent < thisListIndent) {
            return;
        } else if (nextItemIndent > thisListIndent) {
            if (processingSubList) {
                continue;
            }
            processingSubList = true;
            processList(nextList);
        } else {
            processingSubList = false;
            list->add(block);
        }
    }
//     delete nextList;
//     nextList = 0;
}

void NestedListHelper::reformatList(QTextBlock block)
{
    if (block.textList()) {
        int minimumIndent =  block.textList()->format().indent();

        // Start at the top of the list
        while (block.previous().textList() != nullptr) {
            if (block.previous().textList()->format().indent() < minimumIndent) {
                break;
            }
            block = block.previous();
        }

        processList(block.textList());

    }
}

void NestedListHelper::reformatList()
{
    QTextCursor cursor = textEdit->textCursor();
    reformatList(cursor.block());
}

QTextCursor NestedListHelper::topOfSelection() const
{
    QTextCursor cursor = textEdit->textCursor();

    if (cursor.hasSelection()) {
        cursor.setPosition(qMin(cursor.position(), cursor.anchor()));
    }
    return cursor;
}

QTextCursor NestedListHelper::bottomOfSelection() const
{
    QTextCursor cursor = textEdit->textCursor();

    if (cursor.hasSelection()) {
        cursor.setPosition(qMax(cursor.position(), cursor.anchor()));
    }
    return cursor;
}

void NestedListHelper::handleOnIndentMore()
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.beginEditBlock();

    QTextListFormat listFmt;
    if (!cursor.currentList()) {

        QTextListFormat::Style style;
        cursor = topOfSelection();
        cursor.movePosition(QTextCursor::PreviousBlock);
        if (cursor.currentList()) {
            style = cursor.currentList()->format().style();
        } else {

            cursor = bottomOfSelection();
            cursor.movePosition(QTextCursor::NextBlock);

            if (cursor.currentList()) {
                style = cursor.currentList()->format().style();
            } else {
                style = QTextListFormat::ListDisc;
            }
        }
        handleOnBulletType(style);
    } else {
        listFmt = cursor.currentList()->format();
        listFmt.setIndent(listFmt.indent() + 1);

        cursor.createList(listFmt);
        reformatList();
    }
    cursor.endEditBlock();
}

void NestedListHelper::handleOnIndentLess()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextList *currentList = cursor.currentList();
    if (!currentList) {
        return;
    }
    cursor.beginEditBlock();
    QTextListFormat listFmt;
    listFmt = currentList->format();
    if (listFmt.indent() > 1) {
        listFmt.setIndent(listFmt.indent() - 1);
        cursor.createList(listFmt);
        reformatList(cursor.block());
    } else {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.setBlockFormat(bfmt);
        reformatList(cursor.block().next());
    }
    cursor.endEditBlock();
}

void NestedListHelper::handleOnBulletType(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();
    if (styleIndex != 0) {
        QTextListFormat::Style style = static_cast<QTextListFormat::Style>(styleIndex);
        QTextList *currentList = cursor.currentList();
        QTextListFormat listFmt;

        cursor.beginEditBlock();

        if (currentList) {
            listFmt = currentList->format();
            listFmt.setStyle(style);
            currentList->setFormat(listFmt);
        } else {
            listFmt.setStyle(style);
            cursor.createList(listFmt);
        }

        cursor.endEditBlock();
    } else {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.setBlockFormat(bfmt);
    }

    reformatList();
}
