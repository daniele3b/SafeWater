const homepage = require("./routes/homepage"); 
const control = require("./routes/control"); 


module.exports = function (app) {

  app.use("/", homepage);
  app.use("/", control);

};