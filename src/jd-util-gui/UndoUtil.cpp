#include "UndoUtil.h"

namespace JD {
namespace Util {

QUndoCommand *createUndo(const QString &name, UndoFunction &&redo, UndoFunction &&undo, QUndoCommand *parent)
{
	struct Command : public QUndoCommand
	{
		explicit Command(const QString &name, UndoFunction &&redo, UndoFunction &&undo, QUndoCommand *parent = nullptr)
			: QUndoCommand(name, parent), m_redo(std::forward<UndoFunction>(redo)), m_undo(std::forward<UndoFunction>(undo)) {}

		void undo() override { m_undo(); }
		void redo() override { m_redo(); }

	private:
		UndoFunction m_redo;
		UndoFunction m_undo;
	};

	return new Command(name, std::forward<UndoFunction>(redo), std::forward<UndoFunction>(undo), parent);
}


}
}
