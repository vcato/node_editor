#ifndef DIAGRAMNODE_HPP_
#define DIAGRAMNODE_HPP_

#include <vector>
#include <iostream>
#include "textobject.hpp"
#include "nodeindex.hpp"


class DiagramNode {
  public:
    struct Input;
    struct Output;
    struct Line;
    struct Statement;
    struct TextPosition;
    using Outputs = std::vector<Output>;

    std::vector<Input> inputs;
    Outputs outputs;

    std::vector<Line> lines;
    std::vector<Statement> statements;
      // A statement can cross multiple lines.  i.e
      //   return [
      //     10,
      //     20
      //   ]

    DiagramTextObject header_text_object;

    void setText(const std::string &text);
    std::string text() const;
    void setPosition(const DiagramCoords &);
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
    std::vector<std::string> lineTexts() const;

    void joinLines(const TextPosition &);
    void breakLine(const TextPosition &);
    void deleteCharacter(const TextPosition &);
    void insertCharacter(const TextPosition &,char c);

    struct Input {
      int source_node_index = nullNodeIndex();
      int source_output_index = -1;

      bool operator==(const Input &arg) const
      {
        return
          source_node_index==arg.source_node_index &&
          source_output_index==arg.source_output_index;
      }
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

    struct TextPosition {
      int line_index;
      int column_index;

      TextPosition(int line_index_arg,int column_index_arg)
      : line_index(line_index_arg),
        column_index(column_index_arg)
      {
      }

      bool operator==(const TextPosition &arg) const
      {
        return line_index==arg.line_index && column_index==arg.column_index;
      }

      friend std::ostream &
        operator<<(std::ostream &stream,const TextPosition &arg)
      {
        auto l = arg.line_index;
        auto c = arg.column_index;
        stream << "TextPosition(" << l << "," << c << ")";
        return stream;
      }
    };

  private:
    void setNInputs(size_t arg);
    void setNOutputs(size_t arg);
    void updateNInputs();
    void updateNOutputs();
    int inputIndexAt(const TextPosition &position) const;
    void addInputs();
    void addOutputs();

    static size_t countInputs(const DiagramNode &);
      // Should we rename this to countUsedInputs()?

    static size_t countOutputs(const DiagramNode &);
};

#endif /* DIAGRAMNODE_HPP_ */
