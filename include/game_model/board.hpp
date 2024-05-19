#ifndef INCLUDE_GAME_MODEL_BOARD_HPP
#define INCLUDE_GAME_MODEL_BOARD_HPP

#include <array>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <list>

#include "log.h"

using std::unordered_map;
using std::list;
using std::vector;
typedef uint16_t hc_int;

// typedef std::array<hc_int,2> Array2i;

// struct Hasher2i {
//   std::size_t operator()(const Array2i& a) const {
//     // return *((uint32_t*)(a.data()));
//     return *( static_cast<uint32_t*>(a.data()) );
//   }   
// };


class BoardPiece
{
public:
  BoardPiece() {}
  ~BoardPiece() {}

  hc_int& operator[](const hc_int key)
  {
    vector<hc_int>::iterator it = std::find(attribute_keys_.begin(), attribute_keys_.end(), key);
    if (it != attribute_keys_.end()) {
      return attributes_[key];
    }
    else {
      warning("warning: BoardPiece::operator[](const int) used for incorrect attribute value.");
      return attributes_[key];
    }
  }

  const hc_int& Attribute(const hc_int key) const
  { return attributes_.at(key); }
  
  void SetAttribute(const hc_int key, const hc_int val)
  { this->operator[](key) = val; }

  static std::size_t NumAttributes()
  { return attribute_keys_.size(); }

  static vector<hc_int> AttributeKeys()
  { return attribute_keys_; }

  static void AddAttribute(const hc_int key)
  { attribute_keys_.push_back(key); } //TODO check if key already exists

  static void ClearAttributes()
  { attribute_keys_.clear(); }

private:
  hc_int type_ {0};
  std::unordered_map<hc_int, hc_int> attributes_;
  
  static std::vector<hc_int> attribute_keys_;
};


class BoardState
{
public:
  BoardState() {}
  void SetName(const std::string name) { game_name_ = name; }

  // Checking
  bool IsGameFinished() const { return game_finished_; }
  
  // Getting Info
  
  int PositionType(const hc_int index) const
  { return position_map_.at(index); }

  list<BoardPiece>& Pieces(const hc_int index)
  { return piece_map_.at(index); }
  
  list<BoardPiece> Pieces(const hc_int index) const
  { return piece_map_.at(index); }
  
  std::size_t Turn() const
  { return turn_; }

  // Changing game state
  void AddPiece(const BoardPiece piece, const hc_int index);
  void AddPosition(const hc_int type, const hc_int index);
  void EndGame() { game_finished_ = true; }

private:
  // Board Information
  unordered_map<hc_int, list<BoardPiece>> piece_map_;
  unordered_map<hc_int, hc_int> position_map_;
  list<BoardPiece> pieces_;

  // Game State
  std::size_t turn_ = 0;
  bool game_finished_ = false;

  std::string game_name_;

};


#endif