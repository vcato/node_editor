#ifndef NODE2_HPP_
#define NODE2_HPP_

#include <vector>
#include "textobject.hpp"
#include "linetext.hpp"
#include "nodeindex.hpp"


class Node2 {
  public:
    struct Input;
    struct Output;
    struct Line;

    std::vector<Input> inputs;
    std::vector<Output> outputs;
    std::vector<Line> lines;
    TextObject header_text_object;

    void setText(const std::string &text);
    int nLines() const { return lines.size(); }
    int nInputs() const { return inputs.size(); }
    int nOutputs() const { return outputs.size(); }
    void removeLine(int line_index);
    void updateInputsAndOutputs();
    void addInputsAndOutputs();
    bool isEmpty() const;
    std::vector<std::string> strings() const;

    struct Input {
      int source_node_index = nullNodeIndex();
      int source_output_index = -1;
    };

    struct Output {
      float value = 0;
    };

    struct Line {
      std::string text;
      bool has_input = false;
      bool has_output = false;

      Line(const char *text_arg) : text(text_arg) { }
      Line(const std::string &text_arg) : text(text_arg) { }
    };

  private:
    void setNInputs(size_t arg);
    void setNOutputs(size_t arg);
    void updateNInputs();
    void updateNOutputs();
    static size_t countInputs(const Node2 &node);
    static size_t countOutputs(const Node2 &node);
};

#endif /* NODE2_HPP_ */
