#ifndef ANNOTATION_H_
#define ANNOTATION_H_

namespace factory {

namespace annotation {
//marker
struct is_an_annotation
{
private:
    is_an_annotation();
    ~is_an_annotation();
    is_an_annotation( const is_an_annotation& );
    const is_an_annotation& operator=( const is_an_annotation& );
};
struct FREE : private is_an_annotation {
    static std::string name()
    {
        return "FREE";
    }
private:
    FREE();
    ~FREE();
    FREE( const FREE& );
    const FREE& operator=( const FREE& );
};


struct NEWANNOTATION : private is_an_annotation {
    static std::string name()
    {
        return "NEWANNOTATION";
    }
private:
    NEWANNOTATION();
    ~NEWANNOTATION();
    NEWANNOTATION( const NEWANNOTATION& );
    const NEWANNOTATION& operator=( const NEWANNOTATION& );
};

}
}


#endif /* ANNOTATION_H_ */
