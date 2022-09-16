// #define WITH_LIBTMB
// #include <TMB.hpp>

// #include "../inst/include/SAM.hpp"


// #include <R.h>
// #include <Rmath.h>
// #include <Rinternals.h>
// #include <R_ext/Rdynload.h>

struct F_dFunctionalSR2 {
  int srmc;
  double year;
  double lastR;
  vector<double> crb;

  template <template<class> class V, class T>
  T operator()(const V<T> &x){
    confSet conf;
    conf.stockRecruitmentModelCode = srmc;
    conf.constRecBreaks = crb;
    vector<T> rec_pars(x.size()-1);
    for(int i = 0; i < rec_pars.size(); ++i)
      rec_pars(i) = x(i);
    T logssb = x(x.size()-1);
    paraSet<T> par;
    par.rec_pars = rec_pars;
    Recruitment<T> rec = makeRecruitmentFunction(conf, par);
    return rec(logssb, T(year), T(lastR));
  
  }
};

int rec_hasEquilibrium(Recruitment<double>& rec){
    double lse = rec.logSe(0.0);
    return R_finite(lse);
};

int rec_isCompensatory(Recruitment<double>& rec){
  double h = 0.1;
  double x = -5.0;
  bool isComp = !rec.isAutoregressive() && !rec.isTimevarying();
  while(isComp && x < 20){
    if(rec.dSR(x) >= exp(rec(x,R_NaReal,R_NaReal)-x))
      isComp = false;
    x += h;
  }
  return isComp;
};

int rec_hasOvercompensation(Recruitment<double>& rec){
  double h = 0.01;
  double x = -5.0;
  bool isOComp = !rec.isAutoregressive() && !rec.isTimevarying();
  while(!isOComp && x < 20){
    if(rec.dSR(x) < 0)
      isOComp = true;
    x += h;
  }
  return isOComp;
};

int rec_hasMaxAtFiniteS(Recruitment<double>& rec){
  return R_finite(rec.logSAtMaxR());
};

int rec_hasFiniteMax(Recruitment<double>& rec){
  return R_finite(rec.logMaxR());
};

int rec_hasFiniteMaxGradient(Recruitment<double>& rec){
  double g = rec.maxGradient();
  return R_finite(g) && g < 1.0e8;
};

int rec_isAutoregressive(Recruitment<double>& rec){
  return rec.isAutoregressive();
}

int rec_isTimevarying(Recruitment<double>& rec){
  return rec.isTimevarying();
}

extern "C" {
  
  SEXP hcrR(SEXP ssb, SEXP hcrConf){
    vector<double> s = asVector<double>(ssb);
    vector<double> hc = asVector<double>(hcrConf);
    vector<double> r(s.size());
    r.setZero();
    for(int i = 0; i < s.size(); ++i)
      r(i) = hcr(s(i), hc);
    return asSEXP(exp(r));
  }


  SEXP perRecruitR(SEXP logFbar, SEXP tmbdat, SEXP pl, SEXP sel, SEXP aveYears, SEXP nYears, SEXP CT){
    dataSet<double> d0(tmbdat);
    confSet c0(tmbdat);
    paraSet<double> p0(pl);
    vector<double> s0 = asVector<double>(sel);
    vector<double> ls0(s0.size());
    for(int i = 0; i < ls0.size(); ++i)
      ls0 = log(s0);
    vector<int> a0 = asVector<int>(aveYears);
    double logFbar0 = Rf_asReal(logFbar);
    int nY0 = Rf_asInteger(nYears);
    // int RC0 = Rf_asInteger(RC);
    int CT0 = Rf_asInteger(CT);
    PERREC_t<double> y = perRecruit_D<double>(logFbar0, d0, c0, p0, ls0, a0, nY0, CT0);
    const char *resNms[] = {"logF", "logYPR", "logSPR", "logSe", "logRe", "logYe", "dSR0", "logLifeExpectancy", "logYearsLost","logDiscYe","logDiscYPR", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(y.logFbar));
    SET_VECTOR_ELT(res, 1, asSEXP(y.logYPR));
    SET_VECTOR_ELT(res, 2, asSEXP(y.logSPR));
    SET_VECTOR_ELT(res, 3, asSEXP(y.logSe));
    SET_VECTOR_ELT(res, 4, asSEXP(y.logRe));
    SET_VECTOR_ELT(res, 5, asSEXP(y.logYe));
    SET_VECTOR_ELT(res, 6, asSEXP(y.dSR0));
    SET_VECTOR_ELT(res, 7, asSEXP(y.logLifeExpectancy));
    SET_VECTOR_ELT(res, 8, asSEXP(y.logYearsLost));
    SET_VECTOR_ELT(res, 9, asSEXP(y.logDiscYe));
    SET_VECTOR_ELT(res, 10, asSEXP(y.logDiscYPR));

    UNPROTECT(1);    
    return res;

  }

  SEXP perRecruitSR(SEXP logFbar, SEXP tmbdat, SEXP pl, SEXP sel, SEXP aveYears, SEXP nYears, SEXP CT, SEXP logNinit){
    dataSet<double> d0(tmbdat);
    confSet c0(tmbdat);
    paraSet<double> p0(pl);
    vector<double> s0 = asVector<double>(sel);
    vector<double> ls0(s0.size());
    for(int i = 0; i < ls0.size(); ++i)
      ls0 = log(s0);
    vector<int> a0 = asVector<int>(aveYears);
    double logFbar0 = Rf_asReal(logFbar);
    int nY0 = Rf_asInteger(nYears);
    // int RC0 = Rf_asInteger(RC);
    int CT0 = Rf_asInteger(CT);
    vector<double> logNinit0 = asVector<double>(logNinit);
    GetRNGstate();
    PERREC_t<double> y = perRecruit_S<double>(logFbar0, d0, c0, p0, ls0, a0, nY0, CT0, logNinit0);
    PutRNGstate();
    const char *resNms[] = {"logF", "logYPR", "logSPR", "logSe", "logRe", "logYe", "dSR0", "logLifeExpectancy", "logYearsLost","logDiscYe","logDiscYPR", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(y.logFbar));
    SET_VECTOR_ELT(res, 1, asSEXP(y.logYPR));
    SET_VECTOR_ELT(res, 2, asSEXP(y.logSPR));
    SET_VECTOR_ELT(res, 3, asSEXP(y.logSe));
    SET_VECTOR_ELT(res, 4, asSEXP(y.logRe));
    SET_VECTOR_ELT(res, 5, asSEXP(y.logYe));
    SET_VECTOR_ELT(res, 6, asSEXP(y.dSR0));
    SET_VECTOR_ELT(res, 7, asSEXP(y.logLifeExpectancy));
    SET_VECTOR_ELT(res, 8, asSEXP(y.logYearsLost));
    SET_VECTOR_ELT(res, 9, asSEXP(y.logDiscYe));
    SET_VECTOR_ELT(res, 10, asSEXP(y.logDiscYPR));
    
    UNPROTECT(1);    
    return res;

  }



  
  SEXP logSRR(SEXP logssb, SEXP rec_pars, SEXP code, SEXP constRecBreaks, SEXP year, SEXP lastR){
    // Fake paraSet and confSet
    vector<double> rp = asVector<double>(rec_pars);
    paraSet<double> par;
    par.rec_pars = rp;
    int srmc = Rf_asInteger(code);
    vector<double> crb = asVector<double>(constRecBreaks);
    confSet conf;
    conf.stockRecruitmentModelCode = srmc;
    conf.constRecBreaks = crb;
    // Make recruitment
    Recruitment<double> rec = makeRecruitmentFunction(conf, par);
    // Calculate
    int n = Rf_length(logssb);
    SEXP v = PROTECT(Rf_allocVector(REALSXP, n));
    double* LS = REAL(logssb);
    double* LR = REAL(v);
    double* y = REAL(year);
    double* lastRec = REAL(lastR);
    for(int i = 0; i < n; ++i)
      LR[i] = rec(LS[i], y[i], lastRec[i]);
    UNPROTECT(1);
    return v;
  }
 

  SEXP stockRecruitmentModelR(SEXP logssb, SEXP rec_pars, SEXP code, SEXP constRecBreaks, SEXP year, SEXP lastR){
    double b = Rf_asReal(logssb);
    double y = Rf_asReal(year);
    double lr = Rf_asReal(lastR);
     // Fake paraSet and confSet
    vector<double> rp = asVector<double>(rec_pars);
    paraSet<double> par;
    par.rec_pars = rp;
    int srmc = Rf_asInteger(code);
    vector<double> crb = asVector<double>(constRecBreaks);
    confSet conf;
    conf.stockRecruitmentModelCode = srmc;
    conf.constRecBreaks = crb;
    // Make recruitment
    Recruitment<double> rec = makeRecruitmentFunction(conf, par);
    // Calculate	
    double v = rec(b, y, lr);
    F_dFunctionalSR2 Fd = {srmc,y,lr,crb};
    vector<double> x(rp.size() + 1);
    x.setZero();
    for(int i = 0; i < rp.size(); ++i)
      x(i) = rp(i);
    x(x.size()-1) = b;
    TMBad::ADFun<> G(TMBad::StdWrap<F_dFunctionalSR2,vector<TMBad::ad_aug> >(Fd), x);
    G = G.JacFun();
    vector<double> r = G(x);
 
    // vector<double> r = x;
    const char *resNms[] = {"logRecruits", "Gradient","dRdS", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(v));
    SET_VECTOR_ELT(res, 1, asSEXP(r));
    SET_VECTOR_ELT(res, 2, asSEXP(rec.dSR(b)));
    UNPROTECT(1);    
    return res;
      
  }
  
  SEXP recruitmentProperties(SEXP tmbdat, SEXP pl){
    confSet c0(tmbdat);
    paraSet<double> p0(pl);
    Recruitment<double> rec = makeRecruitmentFunction(c0,p0);
    const char *resNms[] = {"name","hasEquilibrium", "isCompensatory", "hasMaxAtFiniteS", "isAutoregressive","isTimevarying","hasOvercompensation","hasFiniteMax","hasFiniteMaxGradient", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, Rf_mkString(rec.name));
    SET_VECTOR_ELT(res, 1, Rf_ScalarLogical(rec_hasEquilibrium(rec)));
    SET_VECTOR_ELT(res, 2, Rf_ScalarLogical(rec_isCompensatory(rec)));
    SET_VECTOR_ELT(res, 3, Rf_ScalarLogical(rec_hasMaxAtFiniteS(rec)));
    SET_VECTOR_ELT(res, 4, Rf_ScalarLogical(rec_isAutoregressive(rec)));
    SET_VECTOR_ELT(res, 5, Rf_ScalarLogical(rec_isTimevarying(rec)));
    SET_VECTOR_ELT(res, 6, Rf_ScalarLogical(rec_hasOvercompensation(rec))); // Must be same as hasMaxAtFiniteS
    SET_VECTOR_ELT(res, 7, Rf_ScalarLogical(rec_hasFiniteMax(rec)));
    SET_VECTOR_ELT(res, 8, Rf_ScalarLogical(rec_hasFiniteMaxGradient(rec)));
    UNPROTECT(1);
    return res;
  }

}
