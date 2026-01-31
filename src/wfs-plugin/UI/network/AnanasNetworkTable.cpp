#include "AnanasNetworkTable.h"

namespace ananas {
    void AnanasNetworkTable::addColumn(const WFS::TableColumns::ColumnHeader &h) const
    {
        table.getHeader().addColumn(h.label, h.id, h.width, h.minWidth, h.maxWidth, h.flags);
    }
} // ananas
