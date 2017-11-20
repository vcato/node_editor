#ifndef NODE2_HPP_
#define NODE2_HPP_

#include <vector>
#include "textobject.hpp"
#include "linetext.hpp"


class Node2 {
  public:
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

    int nInputs() const
    {
      return inputs.size();
    }

    int nOutputs() const
    {
      return n_outputs;
    }

    void removeLine(int line_index)
    {
      lines.erase(lines.begin() + line_index);
    }

    void updateInputsAndOutputs()
    {
      for (auto &line : lines) {
        line.has_input = lineTextHasInput(line.text);
        line.has_output = lineTextHasOutput(line.text);
      }

      updateNInputs();
      updateNOutputs();
    }

    std::vector<std::string> strings() const
    {
      std::vector<std::string> result;

      for (const auto &line : lines) {
        result.push_back(line.text);
      }

      return result;
    }

    std::vector<Input> inputs;
    std::vector<Line> lines;
    TextObject header_text_object;

  private:
    int n_outputs = 0;

    void setNInputs(size_t arg)
    {
      inputs.resize(arg);
    }

    void setNOutputs(size_t arg)
    {
      n_outputs = arg;
    }

    void updateNInputs()
    {
      setNInputs(countInputs(*this));
    }

    void updateNOutputs()
    {
      setNOutputs(countOutputs(*this));
    }

    static size_t countInputs(const Node2 &node)
    {
      size_t n_inputs = 0;

      for (auto &line : node.lines) {
        if (line.has_input) {
          ++n_inputs;
        }
      }

      return n_inputs;
    }

    static size_t countOutputs(const Node2 &node)
    {
      size_t n_outputs = 0;

      for (auto &line : node.lines) {
        if (line.has_output) {
          ++n_outputs;
        }
      }

      return n_outputs;
    }
};

#endif /* NODE2_HPP_ */
