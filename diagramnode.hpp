#ifndef NODE2_HPP_
#define NODE2_HPP_

#include <vector>
#include "textobject.hpp"
#include "nodeindex.hpp"


class DiagramNode {
  public:
    struct Input;
    struct Output;
    struct Line;
    struct Statement;
    using Outputs = std::vector<Output>;

    std::vector<Input> inputs;
    Outputs outputs;
    std::vector<Line> lines;
    std::vector<Statement> statements;
    TextObject header_text_object;

    void setText(const std::string &text);
    void setPosition(const Point2D &);
    const Point2D &position() const { return header_text_object.position; }
    int nLines() const { return lines.size(); }
    int nInputs() const { return inputs.size(); }
    int nOutputs() const { return outputs.size(); }
    void removeLine(int line_index);
    std::vector<bool> determineStatementOutputFlags() const;
    void updateInputsAndOutputs();
    std::string joinLines(int start,int n_lines,char separator) const;
    void addInputsAndOutputs();
    bool isEmpty() const;
    std::vector<std::string> strings() const;

    struct Input {
      int source_node_index = nullNodeIndex();
      int source_output_index = -1;
    };

    struct Output {
    };

    struct Line {
      std::string text;
      int n_inputs = 0;

      Line(const char *text_arg) : text(text_arg) { }
      Line(const std::string &text_arg) : text(text_arg) { }
    };

    struct Statement {
      int n_lines = 0;
      bool has_output = false;
    };

  private:
    void setNInputs(size_t arg);
    void setNOutputs(size_t arg);
    void updateNInputs();
    void updateNOutputs();
    static size_t countInputs(const DiagramNode &);
    static size_t countOutputs(const DiagramNode &);
};

#endif /* NODE2_HPP_ */
