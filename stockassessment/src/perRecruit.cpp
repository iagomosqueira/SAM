#define WITH_LIBTMB
#include <TMB.hpp>
#include "../inst/include/SAM.hpp"


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
    PERREC_t<double> y = perRecruit<double, double>(logFbar0, d0, c0, p0, ls0, a0, nY0, CT0);
    const char *resNms[] = {"logF", "logYPR", "logSPR", "logSe", "logRe", "logYe", "logLifeExpectancy", "logYearsLost","logDiscYe","logDiscYPR", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(y.logFbar));
    SET_VECTOR_ELT(res, 1, asSEXP(y.logYPR));
    SET_VECTOR_ELT(res, 2, asSEXP(y.logSPR));
    SET_VECTOR_ELT(res, 3, asSEXP(y.logSe));
    SET_VECTOR_ELT(res, 4, asSEXP(y.logRe));
    SET_VECTOR_ELT(res, 5, asSEXP(y.logYe));
    SET_VECTOR_ELT(res, 6, asSEXP(y.logLifeExpectancy));
    SET_VECTOR_ELT(res, 7, asSEXP(y.logYearsLost));
    SET_VECTOR_ELT(res, 8, asSEXP(y.logDiscYe));
    SET_VECTOR_ELT(res, 9, asSEXP(y.logDiscYPR));

    UNPROTECT(1);    
    return res;

  }

  SEXP stochPerRecruitR(SEXP logFbar, SEXP tmbdat, SEXP pl, SEXP sel, SEXP aveYears, SEXP nYears, SEXP CT, SEXP logNinit){
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
    PERREC_t<double> y = stochPerRecruit<double>(logFbar0, d0, c0, p0, ls0, a0, nY0, CT0, logNinit0);
    PutRNGstate();
    const char *resNms[] = {"logF", "logYPR", "logSPR", "logSe", "logRe", "logYe", "logLifeExpectancy", "logYearsLost","logDiscYe","logDiscYPR", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(y.logFbar));
    SET_VECTOR_ELT(res, 1, asSEXP(y.logYPR));
    SET_VECTOR_ELT(res, 2, asSEXP(y.logSPR));
    SET_VECTOR_ELT(res, 3, asSEXP(y.logSe));
    SET_VECTOR_ELT(res, 4, asSEXP(y.logRe));
    SET_VECTOR_ELT(res, 5, asSEXP(y.logYe));
    SET_VECTOR_ELT(res, 6, asSEXP(y.logLifeExpectancy));
    SET_VECTOR_ELT(res, 7, asSEXP(y.logYearsLost));
    SET_VECTOR_ELT(res, 8, asSEXP(y.logDiscYe));
    SET_VECTOR_ELT(res, 9, asSEXP(y.logDiscYPR));

    UNPROTECT(1);    
    return res;

  }



  
  SEXP logSRR(SEXP logssb, SEXP rec_pars, SEXP code){
    vector<double> rp = asVector<double>(rec_pars);
    int srmc = Rf_asInteger(code);
    int n = Rf_length(logssb);
    SEXP v = PROTECT(Rf_allocVector(REALSXP, n));
    double* LS = REAL(logssb);
    double* LR = REAL(v);
    for(int i = 0; i < n; ++i)
      LR[i] = functionalStockRecruitment(exp(LS[i]), rp, srmc);
    UNPROTECT(1);
    return v;
  }
  

  SEXP stockRecruitmentModelR(SEXP ssb, SEXP rec_pars, SEXP code){
    double b = Rf_asReal(ssb);
    vector<double> rp = asVector<double>(rec_pars);
    int srmc = Rf_asInteger(code);
	
    double v = exp(functionalStockRecruitment(b, rp, srmc));

#ifdef CPPAD_FRAMEWORK
    vector<AD<double> > rp2(rp.size() + 1);
    for(int i = 0; i < rp.size(); ++i)
      rp2(i) = rp(i);
    rp2(rp.size()) = b;
    CppAD::Independent(rp2);
    // vector<AD<double> > x( 1 );
    // x[0] = b;
    // CppAD::Independent(x);
    vector<AD<double> > y( 1 );
    y[0] = exp(functionalStockRecruitment(rp2(rp.size()), (vector<AD<double> >)rp2.head(rp.size()), srmc));
    CppAD::ADFun<double> F(rp2, y);
    vector<double> x_eval( rp.size() + 1 );
    for(int i = 0; i < rp.size(); ++i)
      x_eval(i) = rp(i);
    x_eval[rp.size()] = b;
    vector<double> r = F.Jacobian(x_eval);
#endif
#ifdef TMBAD_FRAMEWORK
   
    F_dFunctionalSR2 Fd = {rp.size(),srmc};
    vector<double> x(rp.size() + 1);
    for(int i = 0; i < rp.size(); ++i)
      x(i) = rp(i);
    x(rp.size()) = b;
    TMBad::ADFun<> G(TMBad::StdWrap<F_dFunctionalSR2,vector<TMBad::ad_aug> >(Fd), x);
    // TMBad::ADFun<> G(Fd,x);
    G = G.JacFun();
    vector<double> r = G(x);
#endif

    const char *resNms[] = {"Recruits", "Gradient", ""}; // Must end with ""
    SEXP res;
    PROTECT(res = Rf_mkNamed(VECSXP, resNms));
    SET_VECTOR_ELT(res, 0, asSEXP(v));
    SET_VECTOR_ELT(res, 1, asSEXP(r));
 
    UNPROTECT(1);    
    return res;
      
  }

  SEXP Se_sbhR(SEXP lambda, SEXP a, SEXP b, SEXP g){
    double r = Se_sbh(Rf_asReal(lambda), Rf_asReal(a), Rf_asReal(b), Rf_asReal(g));
    return asSEXP(r);
  }

  SEXP Se_slR(SEXP lambda, SEXP a, SEXP b, SEXP g){
    double r = Se_sl(Rf_asReal(lambda), Rf_asReal(a), Rf_asReal(b), Rf_asReal(g));
    return asSEXP(r);
  }

}
