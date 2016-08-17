#ifndef _SOURCE_H
#define _SOURCE_H

// Source class . super class to Light sources
class Source{

	public:

		Source();

		virtual Vect getLightPosition() {
			return Vect(0, 0, 0);
		}

		virtual Color getLightColor() {

			return Color(1, 1, 1, 0);
		}
};

Source::Source() {}

#endif
