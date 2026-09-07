#ifndef PB_FLUX_FROM_TABLE_H
#define PB_FLUX_FROM_TABLE_H

#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Pythia8/Pythia.h"

class PbFluxFromTable : public Pythia8::PDF {
public:
  PbFluxFromTable(int idBeamIn, const std::string& splinePath)
      : Pythia8::PDF(idBeamIn) {
    loadSpline(splinePath);
  }

  void xfUpdate(int, double x, double) override {
    xgamma = interpolate(x);
    xg = xu = xd = xubar = xdbar = xs = xsbar = xc = xcbar = xb = xbbar = 0.0;
    idSav = 9;
  }

private:
  std::vector<double> xLeft_;
  std::vector<double> xRight_;
  std::vector<double> logXLeft_;
  std::vector<double> c0_;
  std::vector<double> c1_;
  std::vector<double> c2_;
  std::vector<double> c3_;

  static double parseFortranNumber(std::string token) {
    for (char& ch : token) {
      if (ch == 'D' || ch == 'd') ch = 'E';
    }
    if (token.find('E') == std::string::npos
        && token.find('e') == std::string::npos) {
      for (std::size_t i = 1; i < token.size(); ++i) {
        if ((token[i] == '+' || token[i] == '-')
            && std::isdigit(static_cast<unsigned char>(token[i - 1]))) {
          token.insert(i, "e");
          break;
        }
      }
    }

    std::size_t parsed = 0;
    double value = std::stod(token, &parsed);
    if (parsed != token.size()) {
      throw std::runtime_error("Could not parse Fortran number: " + token);
    }
    return value;
  }

  void loadSpline(const std::string& splinePath) {
    std::ifstream in(splinePath);
    if (!in) {
      throw std::runtime_error("Could not open photon-flux spline: " + splinePath);
    }

    std::string line;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream iss(line);
      std::string xLeftTok, xRightTok, logXLeftTok, c0Tok, c1Tok, c2Tok, c3Tok;
      if (!(iss >> xLeftTok >> xRightTok >> logXLeftTok
                >> c0Tok >> c1Tok >> c2Tok >> c3Tok)) continue;
      xLeft_.push_back(parseFortranNumber(xLeftTok));
      xRight_.push_back(parseFortranNumber(xRightTok));
      logXLeft_.push_back(parseFortranNumber(logXLeftTok));
      c0_.push_back(parseFortranNumber(c0Tok));
      c1_.push_back(parseFortranNumber(c1Tok));
      c2_.push_back(parseFortranNumber(c2Tok));
      c3_.push_back(parseFortranNumber(c3Tok));
    }

    if (xLeft_.empty()) {
      throw std::runtime_error("Photon-flux spline has no intervals: " + splinePath);
    }
  }

  double interpolate(double x) const {
    if (x <= xLeft_.front() || x >= xRight_.back()) return 0.0;

    auto it = std::upper_bound(xRight_.begin(), xRight_.end(), x);
    if (it == xRight_.end()) return 0.0;

    const std::size_t i = static_cast<std::size_t>(it - xRight_.begin());
    const double dx = std::log(x) - logXLeft_[i];
    const double logValue = ((c3_[i] * dx + c2_[i]) * dx + c1_[i]) * dx + c0_[i];
    return std::exp(logValue);
  }
};

#endif
