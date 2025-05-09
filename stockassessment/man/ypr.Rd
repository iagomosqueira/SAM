% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/tables.R
\name{ypr}
\alias{ypr}
\alias{ypr.sam}
\title{Yield per recruit calculation}
\usage{
ypr(
  fit,
  Flimit = 2,
  Fdelta = 0.01,
  aveYears = min(15, length(fit$data$years)),
  sprProp = 0.35,
  ...
)

\method{ypr}{sam}(
  fit,
  Flimit = 2,
  Fdelta = 0.01,
  aveYears = min(15, length(fit$data$years)),
  sprProp = 0.35,
  ...
)
}
\arguments{
\item{fit}{the object returned from sam.fit}

\item{Flimit}{Upper limit for Fbar}

\item{Fdelta}{increments on the Fbar axis}

\item{aveYears}{Number of years back to use when calculating averages (selection, weights, ...)}

\item{sprProp}{Proportion of SPR at F=0, for example 0.35 if F0.35SPR}

\item{...}{extra arguments not currently used}
}
\description{
Yield per recruit calculation
}
