#ifndef NODE2_HPP_
#define NODE2_HPP_

#include <vector>
#include "textobject.hpp"
#include "linetext.hpp"


class Node2 {
  public:
    struct Input;
    struct Line;

    std::vector<Input> inputs;
    std::vector<Line> lines;
    TextObject header_text_object;

    void setText(const std::string &text);
    int nInputs() const { return inputs.size(); }
    int nOutputs() const { return n_outputs; }
    void removeLine(int line_index);
    void updateInputsAndOutputs();
    bool isEmpty() const;
    std::vector<std::string> strings() const;

    struct Input {
      int source_node_index = -1;
      int source_output_index = -1;
    };

    struct Line {
      std::string text;
      bool has_input = false;
      bool has_output = false;

      Line(const char *text_arg) : text(text_arg) { }
      Line(const std::string &text_arg) : text(text_arg) { }
    };

  private:
    int n_outputs = 0;

    void setNInputs(size_t arg);
    void setNOutputs(size_t arg);
    void updateNInputs();
    void updateNOutputs();
    static size_t countInputs(const Node2 &node);
    static size_t countOutputs(const Node2 &node);
};

#endif /* NODE2_HPP_ */
