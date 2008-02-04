TARGETS = \
	mootm.taxi2.gfsc \
	mootm.taxi3.gfsc \
	mootm.taxi3.bytype.gfsc \
	\
	lts-cascade.id.gfsc \
	lts-cascade.no-subst.gfsc \
	lts-cascade.gfsc \
	\
	mootm.robust.no-subst.gfsc \
	mootm.robust.gfsc

#	mootm.taxi.gfsc \
#	mootm.lo.editor.gfsc
#	mootm.robust.immediate.no-subst.gfsc \
#	mootm.robust.immediate.gfsc \
#	mootm.robust.no-insert.gfsc \
#	mootm.robust.max1.gfsc
#	mootm.gfsc

KNOWN_TARGETS = \
	lts-grimm.letters.gfst \
	lts-grimm.letters.inv.gfst \
	lts-mootm.gfst \
	\
	mootm.lo.editor.gfsc \
	mootm.robust.no-subst.gfsc \
	mootm.robust.gfsc \
	mootm.robust.immediate.no-subst.gfsc \
	mootm.robust.immediate.gfsc \
	mootm.robust.no-insert.gfsc \
	mootm.robust.max1.gfsc \
	mootm.gfsc


#CASCADE_COMPILE = ../../src/programs/gfsmxl_cascade_compile
CASCADE_COMPILE  = gfsmxl_cascade_compile

all: $(TARGETS)

##----------------------------------------------------------------------
## labels
mootm.lo.lab: mootm.gfst mootm.lab
	./robust/gfsm-extract-alphabet.perl -lower -noupper -l mootm.lab -o $@ mootm.gfst

lts-grimm.lo.lab: lts-grimm.gfst lts-grimm.lab
	./robust/gfsm-extract-alphabet.perl -lower -noupper -l lts-grimm.lab -o $@ lts-grimm.gfst

lts-grimm.hi.lab: lts-grimm.gfst lts-grimm.lab
	./robust/gfsm-extract-alphabet.perl -nolower -upper -l lts-grimm.lab -o $@ lts-grimm.gfst

%.lab %.scl: %.sym
	lexmakelab $*

##----------------------------------------------------------------------
## Unigram-Alignment editor

##-- v1
mootm.editor.taxi.gfst: taxi-align.taxi-words.text.align.tfst mootm.lab
	gfsmcompile -i mootm.lab -o mootm.lab -F $@ $< 

mootm.taxi.gfsc: mootm.editor.taxi.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##-- v2
mootm.editor.taxi2.gfst: taxi-align.taxi-words.text.align2.tfst mootm.lab
	gfsmcompile -i mootm.lab -o mootm.lab -F $@ $< 

mootm.taxi2.gfsc: mootm.editor.taxi2.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##-- v3
mootm.editor.taxi3.gfst: taxi-align.taxi-words.text.align3.tfst mootm.lab
	gfsmcompile -i mootm.lab -o mootm.lab -F $@ $< 

mootm.taxi3.gfsc: mootm.editor.taxi3.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##-- v3, bytype
mootm.editor.taxi3.bytype.gfst: taxi-align.taxi-words.text.align3.bytype.tfst mootm.lab
	gfsmcompile -i mootm.lab -o mootm.lab -F $@ $< 

mootm.taxi3.bytype.gfsc: mootm.editor.taxi3.bytype.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##----------------------------------------------------------------------
## LTS: letters-only

lts-letters.afst: lts-letters.re lts-grimm.lab lts-grimm.scl
	lexcompre -l lts-grimm.lab -S lts-grimm.scl -s "`cat $<`" -F $@

%.gfst.afst: %.gfst
	gfsmprint $< | fsmcompile -t -F $@

lts-grimm.letters.afst: lts-letters.afst lts-grimm.gfst.afst
	fsmcompose $^ | fsmcompact -F $@

lts-grimm.letters.inv.afst: lts-grimm.letters.afst
	fsminvert -F $@ $<

%.gfst: %.afst
	fsmprint $< | gfsmcompile -F $@

lts: lts-grimm.letters.gfst lts-grimm.letters.inv.gfst

##----------------------------------------------------------------------
## LTS: cascades

##-- lts-mootm.gfst: morphology using LTS orthographic input labels
lts-mootm.gfst: mootm.gfst mootm.lo.lab lts-grimm.lo.lab
	./robust/gfsm-labconvert.perl $^ $@

##-- lts-mootm.pho.afst: phonetic morphology [compact = KABOOM!]
lts-mootm.pho.afst: lts-mootm.gfst.afst lts-grimm.letters.afst
	fsmcompose $^ | fsmproject -2 -F $@

##-- lts-grimm.hi.editor.gfst: lts-phonetic editor
lts-grimm.hi.editor.gfst: lts-grimm.hi.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -cost-subst=1 -delayed   -o $@ $<

lts-grimm.hi.editor.no-subst.gfst: lts-grimm.hi.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -no-subst     -delayed   -o $@ $<

##-- (LTS --> LTS^(-1) --> morph): identity cascade
lts-cascade.id.gfsc: lts-grimm.letters.gfsx lts-grimm.letters.inv.gfsx lts-mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##-- (LTS --> PHO_EDITOR --> LTS^(-1) --> morph): editor cascade
lts-cascade.gfsc:  lts-grimm.letters.gfsx lts-grimm.hi.editor.gfsx lts-grimm.letters.inv.gfsx lts-mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

lts-cascade.no-subst.gfsc:  lts-grimm.letters.gfsx lts-grimm.hi.editor.no-subst.gfsx lts-grimm.letters.inv.gfsx lts-mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##-- (LTS --> PHO_EDITOR --> PHO_MORPH): editor cascade to phonetic target language
lts-cascade.pho.gfsc: lts-grimm.letters.gfsx lts-grimm.hi.editor.gfsx lts-mootm.pho.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

lts-cascade.pho.no-subst.gfsc: lts-grimm.letters.gfsx lts-grimm.hi.editor.no-subst.gfsx lts-mootm.pho.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)


##----------------------------------------------------------------------
## editors: orthorgraphic
mootm.lo.editor.gfst: mootm.lo.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -cost-subst=1 -delayed   -o $@ $< 

mootm.lo.editor.no-subst.gfst: mootm.lo.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -no-subst     -delayed -o $@ $< 

mootm.lo.editor.immediate.gfst: mootm.lo.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -cost-subst=1 -immediate -o $@ $< 

mootm.lo.editor.immediate.no-subst.gfst: mootm.lo.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -no-subst     -immediate -o $@ $< 

mootm.lo.editor.max1.gfst: mootm.lo.lab
	./robust/gfsm-make-editor.perl -cost-match=0 -cost-insert=1 -cost-delete=1 -cost-subst=1 -delayed -max-cost=1 -o $@ $< 

##----------------------------------------------------------------------
## "robust" morphology cascades
mootm.robust.gfsc: mootm.lo.editor.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

mootm.robust.%.gfsc: mootm.lo.editor.%.gfsx mootm.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)


##----------------------------------------------------------------------
## automaton indexing & singleton-cascade generation
%.gfsx: %.gfst
	gfsmindex -F $@ $<

%.gfsc: %.gfsx
	$(CASCADE_COMPILE) $^ -F $@ || (rm -f $@; false)

##----------------------------------------------------------------------
## cleanup
clean:
	rm -f $(TARGETS)

realclean: clean
	rm -f $(KNOWN_TARGETS)
	rm -f *~ .*~
	rm -f *.afst *.gfsc *.gfsx *.editor*.gfst
	rm -f lts*.lab lts*.scl mootm*.lab mootm*.scl
