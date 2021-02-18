#ifndef patchhandler_handler_h_
#define patchhandler_handler_h_


//#define DEBUG_PATCH_HANDLER

class patchHandler {

  public:
    patchHandler();
    ~patchHandler(){};

    bool getParamValue(const char *l_device, const char *l_param, float &val);

  private:
#ifdef DEBUG_PATCH_HANDLER
    char str_[100];
#endif
};


#endif /*patchhandler_handler_h_*/
