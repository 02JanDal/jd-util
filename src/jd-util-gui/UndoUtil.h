#pragma once

#include <QUndoCommand>

#include <functional>

namespace JD {
namespace Util {

using UndoFunction = std::function<void()>;
QUndoCommand *createUndo(const QString &name, UndoFunction &&redo, UndoFunction &&undo, QUndoCommand *parent = nullptr);


}
}
