#include "game_model/board.hpp"
#include "log.h"


void BoardState::AddPiece(const BoardPiece piece, const hc_int index)
{
  piece_map_.at(index).push_back(piece);
}

void BoardState::AddPosition(const hc_int type, const hc_int index)
{
  if (position_map_.find(index) != position_map_.end()) {
    warning("Added multiple positions to same board coordinate.");
  }
  position_map_[index] = type;
}
